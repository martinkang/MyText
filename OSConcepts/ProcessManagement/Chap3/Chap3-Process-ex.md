# 연습문제
* 3.1 아래 각각의 장점과 단점은 무엇인가? 시스템 프로그래머의 관점을 모두 고려하시오
	- a. 동기적 통신과 비동기적 통신
		- 장점
			- 동기적 통신은 동기화를 신경쓰지 않아도 된다.
		- 단점

			- 동기적 통신은 봉쇄당하는 동안 다른 작업을 할 수 없다.
	- b. 자동과 명시적 버퍼링
		- 장점
			- 자동 버퍼링은 효율적인 자원 활용과 구현이 편하다.
		- 단점
			- 때에 따라 명시적 버퍼링보다 성능이 안좋다.
				- 명시적 버퍼링은 상황에 맞게 튜닝을 하여 좋은 성능을 가질 수 있다.
				- 명시적 버퍼링은 구현이 어렵다.
	- c. 복사에 의한 송신과 참조에 의한 송신
		- 장점
			- 참조에 의한 송신은 복사에 의한 비용이 없다.
		- 단점
			- 참조에 의한 송신은 데이터 오염의 가능성이 있다.
	- d. 고정-크기와 가변-크기 메시지
		- 장점
			- 고정-크기 메시지는 구현이 쉽고, 메시지 길이가 정해져 있기 때문에 
			추가적인 연산을 필요로 하지 않아 오버헤드가 적다.
		- 단점
			- 메시지 크기가 정해져 있기 때문에 장문의 메시지의 경우 여러번에 나눠 보내고 
			수신하는 쪽에서 이를 합치는 연산이 따로 필요하다.
			- 매우 작은 크기의 메시지일 경우 공간 낭비가 심하다.
* 3.2 RPC 기법에서 "최대 한 번" 이나 "정확히 한 번" 의 의미를 강제하지 않음으로써 야기될 수 있는 
바람직하지 못한 결과를 기술하시오. 이러한 보장이 없는 기법을 사용할 수 있는 예를 기술하시오
	- 야기될 수 있는 문제
		- 메시지 수신측이 데이터를 못받을 수 있고, 만약 봉쇄형 수신이라면 수신측은 계속 block 상태로 대기할 것이다.
		- 데이터를 신뢰할 수 없게 된다.
	- 사용할 수 있는 예
		- 정확성보다 실시간 반응성을 요구하는 경우
			- ex ) 통화 음질이 조금 떨어지더라도 실시간 성이 더 중요하다.
* 3.4 병행 처리가 운영체제에게 부가하는 주요 복잡성 3가지
	- 동기화 문제
	- 공유 문제
	- 보안 문제
* 3.5 프로세스들 사이에 문맥을 교환하기 위해 커널이 실행하는 작업을 설명하시오
	- Context Switching
		- 현재 실행중인 Context 를 해당 PCB 에 저장하고 상태를 준비상태로 변경
		- 실행할 Context 를 메모리에 적재하고 PCB 상태를 실행으로 변경
		- 실행이 완료되면 이전에 실행하던 Context 를 다시 메모리에 적재하여 실행
	- 커널은 해당 프로세스의 정보를 PCB 에 저장하고 스케줄 될 새로운 프로세스를 복구시키는 역할을 한다.
* 3.6 Context Switching 이 일어날 때 새로운 Context 가 이미 레지스터 집합 중 하나에 적재되어 있다면 어떤 일이 일어날 까? 모든 레지서트 집합이 사용 중이고 새 문맥이 메모리에 있을 때에는 어떤 일이 일어나는가?
	- 이미 적재되어 있다면
		- 현재 레지스터 포인터만 변경한다.
	- 레지스터가 꽉 차 있는 경우
		- 레지스터 중 하나의 내용을 메모리로 내리고, 새 Context 를 레지스터에 적재
* 3.8 익명 파이프가 네임드 파이프보다 더 적합한 상황의 예를 들어보시오. 반대로 네임드 파이프가 더 적당한 상황의 예를 들어 보시오
	- 익명 파이프는 자식 프로세스의 작업 진행상황등을 통신할 때 유용
	- 네임드 파이프는 서버-클라이언트 모델에서 유용
* 3.9 단기, 중기, 장기 스케줄러 차이점 설명
	- 단기 스케줄러는 실행 준비과 완료된 프로세스를 선택하는 스케줄러로 매우 빠르고 빈번하게 실행된다.
	- 장기 스케줄러는 실행할 프로세스들을 메모리로 적재하고, 
	입출력 중심/CPU 중심 작업을 조절하여 CPU 자원 효율을 높인다. 실행 빈도가 적다.
	- 중기 스케줄러는 CPU 경쟁이 과부화 되는걸 막기 위해 메모리에서 프로세스를 디스크로 내려, 
	다중 프로그래밍의 정도를 완화시킨다. 차후에 제거했던 프로세스를 다시 메모리로 적재하는 과정을 재개한다.
