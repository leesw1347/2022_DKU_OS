1. 큐와 해시 자료 구조 공유

## 1. 큐쓰레드 과제

2. https://github.com/DKU-EmbeddedSystem-Lab/2022_DKU_OS
   다중 쓰레드로 구성된 프로그램 작성 (생산자 소비자 문제 구조)
   ### 모든 쓰레드는 큐와 해시 자료 구조 공유
   ### 일부 쓰레드는 키 추가/삭제 수행
   ### 다른 일부 쓰레드는 키 검색 수행 -- 이때 큐와 해시에
    1) 락이 있을 때와 없을 때,
    2) coarse vs fine-grained lock일 때,
    3) 쓰레드 개수가 달라질 때 (예를들어 10~100개) 수행 결과 비교

- 개발 환경: 리눅스 환경 (Virtual box 상에 ubuntu 설치)
- 보고서 요구사항: 1) 목표, 2) 설계/구현, 3) 수행 결과 (3가지 비교), 4) 논의

centos 기본셋팅 sudo yum install openssl-devel

ubuntu 기본 셋팅 sudo apt-get update && sudo apt-get upgrade sudo apt-get install gcc g++ -y sudo apt-get install wget sudo apt-get install build-essential sudo apt-get install openssl sudo apt-get install openssh-server
sudo apt-get install libssl-dev cmake 최신버전 설치 sudo apt-get install https://github.com/Kitware/CMake/releases/download/v3.23.1/cmake-3.23.1.tar.gz
./bootstrap --prefix=/usr make sudo make install make 설치