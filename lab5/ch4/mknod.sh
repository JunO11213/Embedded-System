#!/bin/sh

MODULE="ch4_dev"

rm -f /dev/$MODULE

MAJOR=$(awk -v mod="$MODULE" '$2 == mod {print $1}' /proc/devices)

if [ -z "$MAJOR" ]; then
    echo "에러: 커널에서 '$MODULE'을 찾을 수 없습니다."
    echo "sudo insmod ch4_mod_202111360.ko 명령어를 먼저 실행했는지 확인하세요!"
    exit 1
fi

echo "메이저 번호 [$MAJOR]번으로 /dev/$MODULE 디바이스 파일을 생성합니다..."
mknod /dev/$MODULE c $MAJOR 0

chmod 666 /dev/$MODULE

echo "디바이스 파일 생성 완료! 이제 테스트를 진행하세요."