#!/bin/sh

MODULE="ch3_dev"

# 1. 기존에 잘못 만들어진 파일이 있다면 찌꺼기 삭제 (File exists 에러 방지)
rm -f /dev/$MODULE

# 2. awk 명령어 따옴표 충돌을 막기 위해 더 안전한 방식(-v)으로 메이저 번호 찾기
MAJOR=$(awk -v mod="$MODULE" '$2 == mod {print $1}' /proc/devices)

# 3. 메이저 번호를 못 찾은 경우 (모듈 안 올리고 실행했을 때 에러 방지)
if [ -z "$MAJOR" ]; then
    echo "에러: 커널에서 '$MODULE'을 찾을 수 없습니다."
    echo "sudo insmod ch3_mod_학번.ko 명령어를 먼저 실행했는지 확인하세요!"
    exit 1
fi

# 4. 디바이스 파일 생성
echo "메이저 번호 [$MAJOR]번으로 /dev/$MODULE 디바이스 파일을 생성합니다..."
mknod /dev/$MODULE c $MAJOR 0

# 5. 일반 유저(reader, writer)도 접근할 수 있게 권한 열어주기 (핵심!)
chmod 666 /dev/$MODULE

echo "디바이스 파일 생성 완료! 이제 테스트를 진행하세요."