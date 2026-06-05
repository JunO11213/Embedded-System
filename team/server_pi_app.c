/*
 * server_pi_app.c
 * Pi2 user application
 *
 * Roles:
 *   - Connect to local Mosquitto broker
 *   - Subscribe entry/request and exit/request
 *   - Manage current_count with mutex
 *   - Publish ALLOW/DENY responses
 *   - Control inside LED via /dev/pi2_led_dev
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>

#include <mosquitto.h>

#define BROKER_HOST "localhost"
#define BROKER_PORT 1883
#define KEEPALIVE   60

#define MAX_COUNT 5
#define LED_DEV_PATH "/dev/pi2_led_dev"

#define TOPIC_ENTRY_REQUEST  "entry/request"
#define TOPIC_EXIT_REQUEST   "exit/request"
#define TOPIC_ENTRY_RESPONSE "entry/response"
#define TOPIC_EXIT_RESPONSE  "exit/response"
#define TOPIC_SYSTEM_STATUS  "system/status"

static volatile sig_atomic_t running = 1;
static int current_count = 0;
static int led_fd = -1;
static pthread_mutex_t count_lock = PTHREAD_MUTEX_INITIALIZER;

static void handle_signal(int sig)
{
    (void)sig;
    running = 0;
}

static int payload_equals(const struct mosquitto_message *msg, const char *s)
{
    int len = (int)strlen(s);

    if (msg == NULL || msg->payload == NULL)
        return 0;

    if (msg->payloadlen != len)
        return 0;

    return memcmp(msg->payload, s, len) == 0;
}

static void set_inside_led(int on)
{
    const char value = on ? '1' : '0';
    ssize_t ret;

    if (led_fd < 0) {
        fprintf(stderr, "[LED] device is not open\n");
        return;
    }

    ret = write(led_fd, &value, 1);
    if (ret < 0) {
        perror("write LED device");
    }
}

static void update_inside_led(void)
{
    set_inside_led(current_count > 0);
}

static void publish_status(struct mosquitto *mosq)
{
    char status[128];
    int full = current_count >= MAX_COUNT ? 1 : 0;

    snprintf(status, sizeof(status),
             "count=%d,max=%d,full=%d",
             current_count, MAX_COUNT, full);

    mosquitto_publish(mosq, NULL, TOPIC_SYSTEM_STATUS,
                      (int)strlen(status), status, 0, false);

    printf("[STATUS] %s\n", status);
}

static void handle_entry_request(struct mosquitto *mosq)
{
    if (current_count < MAX_COUNT) {
        current_count++;

        mosquitto_publish(mosq, NULL, TOPIC_ENTRY_RESPONSE,
                          5, "ALLOW", 0, false);

        printf("[ENTRY] ALLOW, current_count=%d\n", current_count);
    } else {
        mosquitto_publish(mosq, NULL, TOPIC_ENTRY_RESPONSE,
                          4, "DENY", 0, false);

        printf("[ENTRY] DENY, full, current_count=%d\n", current_count);
    }
}

static void handle_exit_request(struct mosquitto *mosq)
{
    if (current_count > 0) {
        current_count--;

        mosquitto_publish(mosq, NULL, TOPIC_EXIT_RESPONSE,
                          5, "ALLOW", 0, false);

        printf("[EXIT] ALLOW, current_count=%d\n", current_count);
    } else {
        mosquitto_publish(mosq, NULL, TOPIC_EXIT_RESPONSE,
                          4, "DENY", 0, false);

        printf("[EXIT] DENY, current_count=0\n");
    }
}

static void on_connect(struct mosquitto *mosq, void *obj, int rc)
{
    (void)obj;

    if (rc == 0) {
        printf("[MQTT] connected to broker\n");
        mosquitto_subscribe(mosq, NULL, TOPIC_ENTRY_REQUEST, 0);
        mosquitto_subscribe(mosq, NULL, TOPIC_EXIT_REQUEST, 0);
        printf("[MQTT] subscribed: %s, %s\n",
               TOPIC_ENTRY_REQUEST, TOPIC_EXIT_REQUEST);
    } else {
        fprintf(stderr, "[MQTT] connection failed rc=%d\n", rc);
    }
}

static void on_message(struct mosquitto *mosq, void *obj,
                       const struct mosquitto_message *msg)
{
    (void)obj;

    printf("[MQTT] recv topic=%s payload=%.*s\n",
           msg->topic,
           msg->payloadlen,
           msg->payload ? (char *)msg->payload : "");

    pthread_mutex_lock(&count_lock);

    if (strcmp(msg->topic, TOPIC_EXIT_REQUEST) == 0 &&
        payload_equals(msg, "EXIT")) {
        handle_exit_request(mosq);
    } else if (strcmp(msg->topic, TOPIC_ENTRY_REQUEST) == 0 &&
               payload_equals(msg, "ENTER")) {
        handle_entry_request(mosq);
    } else {
        printf("[WARN] unknown message ignored\n");
    }

    update_inside_led();
    publish_status(mosq);

    pthread_mutex_unlock(&count_lock);
}

int main(void)
{
    struct mosquitto *mosq;
    int rc;

    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    led_fd = open(LED_DEV_PATH, O_RDWR);
    if (led_fd < 0) {
        fprintf(stderr, "failed to open %s: %s\n", LED_DEV_PATH, strerror(errno));
        fprintf(stderr, "check: sudo insmod pi2_led_driver.ko && sudo sh mknod.sh\n");
        return 1;
    }

    set_inside_led(0);

    mosquitto_lib_init();

    mosq = mosquitto_new("pi2_server_app", true, NULL);
    if (mosq == NULL) {
        fprintf(stderr, "mosquitto_new failed\n");
        close(led_fd);
        mosquitto_lib_cleanup();
        return 1;
    }

    mosquitto_connect_callback_set(mosq, on_connect);
    mosquitto_message_callback_set(mosq, on_message);

    rc = mosquitto_connect(mosq, BROKER_HOST, BROKER_PORT, KEEPALIVE);
    if (rc != MOSQ_ERR_SUCCESS) {
        fprintf(stderr, "mosquitto_connect failed: %s\n", mosquitto_strerror(rc));
        mosquitto_destroy(mosq);
        close(led_fd);
        mosquitto_lib_cleanup();
        return 1;
    }

    printf("Pi2 server application started. MAX_COUNT=%d\n", MAX_COUNT);

    while (running) {
        rc = mosquitto_loop(mosq, 1000, 1);
        if (rc != MOSQ_ERR_SUCCESS) {
            fprintf(stderr, "mosquitto_loop error: %s\n", mosquitto_strerror(rc));
            sleep(1);
            mosquitto_reconnect(mosq);
        }
    }

    printf("Pi2 server application stopping...\n");

    set_inside_led(0);

    mosquitto_disconnect(mosq);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();

    close(led_fd);
    return 0;
}
