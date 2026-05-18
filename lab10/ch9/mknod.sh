#!/bin/bash

# 학번 부분은 실제 생성된 .ko 파일명에 맞게 수정하세요.
MODULE_NAME="ch9_mod_학번"

echo "1. 기존 커널 메시지 초기화 중..."
sudo dmesg -C

echo "2. 스테퍼 모터 모듈($MODULE_NAME.ko) 적재 (구동 시작)..."
sudo insmod $MODULE_NAME.ko

echo "3. 커널 메시지(dmesg) 확인:"
sudo dmesg

echo "4. 모듈 제거 중..."
sudo rmmod $MODULE_NAME.ko
echo "모듈 제거 완료."