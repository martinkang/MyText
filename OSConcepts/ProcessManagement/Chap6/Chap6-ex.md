# 연습문제
* 6.2 다중 처리기 시스템을 위한 동기화 프리미티브를 구현할 때 인터럽트를 사용하는 것이 부적합한 이유
	- 공유 변수가 변경되는 동안 인터럽트가 발생하면 공유변수의 예측 못한 변경이 일어날 수 있다.
		- 만약 A 라는 변수를 변경하는 동안 인터럽트가 발생하하여
		인터럽트 핸들러가 이 A 를 수정한다면? A 의 값 변화를 예측 할 수 없게 된다.
* 6.8 인터럽트를 빈번하게 비활성화 시키는 것은 시스템의 클록에 영향을 줄 수 있다고 언급하였다.
이러한 결과가 발생하는 이유를 설명하고 이러한 영향을 최소화할 수 있는 해결책을 설명하시오.
	- 인터럽트 불능화는 상당한 시간을 소요한다.
		- 이러한 메시지가 모든 프로세서에게 전달되어야 한다.
		- 이러한 메시지 전달은 매 임계 영역에 진입 하는 것을 지연시켜, 시스템 효율을 떨어뜨린다.
		- 인터럽트에 의해 클록이 갱신된다면, 시스템 클록에 대한 영향을 줄 수 있다.
	- 해결안
		- 원자적인 작업 단위를 하드웨어/소프트웨어 적으로 제공하여 인터럽트가 발생함하여도
		변수의 예측 못한 변경이 없도록 한다.
* 6.11 wait() 과 signal() 연산이 원자적으로 실행되지 않으면 상호 배제가 보장될 수 없음을 보이시오.
```c++
/* wait() 연산 */
void wait( semaphores *S )
{
	S->value--;
	if ( s->value < 0 )
	{
		s->list.put( P );
		block();
	}
}

/* signal() 연산 */
void signal( semaphores *S )
{
	S->value++;
	if ( S->value <= 0 )
		S->list.pop( &P ); // list 에서 프로세스 P 를 끄낸다.
		wakeup( P );
	}
}
```
* 상호 배제가 보장될 수 없는 경우.
	1. wait() : S->value--;  
	2. wait() : S->value 가 -1 이 되어 if 문 조건 ( s->value < 0 ) 을 만족  
	3. wait() : s->list.put( P ) 를 하여 프로세스 P 가 list 에 추가됨.
	4. signal() : S->value++;
	5. signal() : S->value 가 0 이 되어 if 문 조건 ( s->value <= 0 ) 을 만족
	6. signal() : S->list.pop( &P ) 를 하여 3 번 과정에서 put 한 프로세스 P 를 가져옴
	7. signal() : P 에 wakeup 시그널을 보내지만 P 는 아직 block() 상태가 아니라 아무일도 발생하지 않음.
	8. wait() : 프로세스 P block() 이 되어 다시 signal() 을 하기 전까지 깨어나지 못함.
* 6.12 다중처리기 환경에서 TestAndSet() 명령을 사용하여 wait() 와 signal() 세마포 연산을 구현하는 방법을 보이시오.
해결 방안은 바쁜 대기를 최소로 실행 해야 한다.
```c++
boolean lock = FALSE;

struct sem
{
	int value; // 세마포 카운트
	plist list; // 대기중인 Process list
}

/* wait */
wait( struct sem s )
{
	boolean key = TestAndSet( &lock );

	while ( key == TRUE  )
	{
		key = TestAndSet( &lock );
	}

	s->value--;

	if ( s->value < 0 )
	{
		s->list.put( P );
		while ( s->value < 0 )
		{
			/* busy waiting */
		}
	}

	lock = TRUE;
}

/* signal */
signal( struct sem s )
{
	boolean key = TestAndSet( &lock );

	while ( key == TRUE  )
	{
		key = TestAndSet( &lock );
	}

	s->value++;
	if ( s->value <= 0 )
		s->list.pop( &P );
	}

	lock = FALSE;
}
```
* 6.14 두 단계 락킹 프로토콜이 충돌 직렬가능성을 보장하는 것을 보이시오.
	-
* 6.16 휘발성, 비휘발성, 안전 저장장치의 차이점을 비용 측면에서 설명하시오.
	- 휘발성
		- 전원이 내려가면 저장된 정보가 사라짐
		- 매우 빠름
		- 용량비 비쌈
	- 비휘발성
		- 전원이 내려가도 정보가 사라지지 않음
		- 느린편
		- 용량비 쌈
	- 안전 저장장치
		- 정보가 손실될 일이 없음 ( 거의 )
* 6.19 경쟁 조건이 발생할 가능성이 있는 커널 자료구조를 기술하시오.
경쟁 조건이 어떤 경우에 발생할 수 있는 지에 대한 설명을 반드시 포함하라.
	-
* 6.21 다중처리기에서는 spinlock 이 종종 사용되는 데 비해 단일처리기 시스템에서는 부적당한 이유를 설명하시오.
	- spinlock 은 lock 을 획득할 수 없어 대기하는 동안 cpu 를 사용하면서 lock 을 획득할 수 있을때까지 loop 를 돌고있다.
	단일 처리기에서는 처리기가 단 하나뿐이므로 lock 을 반납하는 처리기와 획득하는 처리기가 결국 같은 처리기므로
	lock 을 획득하기 위해 하나뿐인 처리기를 사용하여 lock 을 기다리는 행위는 매우 부적절하다.
* 6.24 Windows Vista 는 slim reader-writer 락이라고 불리는 새로운 경량 동기화 도구를 제공한다.
대부분읜 reader-wirter 락 구현이 reader 또는 writer 를 선호하거나 FIFO 정책에 따라 스레드의 대기 순서를 정하는데 반해,
slim reader-writer 락은 어느 쪽도 선호하지 않거나 FIFO 큐를 사용하여 대기 순서를 정하지도 않는다.
이러한 동기화 도구를 제공할 때 어떤 이점이 있는가?
	-
* 6.25 롤백된 트랜잭션에게 새 타임스탬프를 배정하는 것은 무슨 의미가 있는가?
롤백 트랜잭션 보다 이후에 시작하였지만 롤백 트랜잭션의 새 타임스탬프보다 작은 스탬프를 가지고 있는
트랜잭션들은 어떻게 처리되는가?
	- 새 타임스탬프를 배정하는 것의 의미
		- 롤백 트랜잭션의 새 타임스탬프보다 작은 타임 스탬프를 가진 트랜잭션들의
	- 롤백 트랜잭션보다 이후에 시작하였지만 롤백 트랜잭션의 새 타임스탬프보다 작은 스탬프를 가지고 있을 때
		- cascading rollback 이 일어난다. ( 연쇄 복구 )
		- 아래 그림에서 T1 이 Write(a) 를 하고 T2 가 Write(a) 를 하였을 때 T1 이 Rollback 이 발생하여
		T1 의 새 타임스탬프는 Rollback 시점이 된다. 이럴 경우
	-
![rollback](http://dbscthumb.phinf.naver.net/4515_000_1/20160715113240063_UTCSRGS35.jpg/ka26_216_i1.jpg?type=w690_fst_n&wm=Y)
	- 출처 : http://terms.naver.com/entry.nhn?docId=3431280&cid=58430&categoryId=58430
* 6.26 Reader-writer 문제에서 공평성과 처리량 간의 trade-off 를 논의하시오.
기아 현상을 유발하지 않는 readers-wirters 문제의 해결책을 제시하시오.
	- Reader-Writer 문제에서 공평성과 처리량 간의 trade-off
		-
	- 기아 현상을 유발하지 않는 해결책
		-
* 6.28 바쁜 대기 ( busy-waiting ) 의 의미는 무엇인가? 운영체제 안에서 이 방식과 다르게 기다리는 방식은 무엇인가?
바쁜 대기를 전혀 사용하지 않을 수 있는가?
	- Busy-Waiting
		- lock 획득을 시도해보고 lock 획득을 하지 못하면 cpu 를 점유한채 lock 이 획득될때까지 loop 를 돌며 대기 & lock 획득 시도 를 한다.
		- Context Switching 이 발생하지 않기 때문에 효율적이지만, CPU 를 계속 점유하고 있기 때문에 lock 대기 시간이 길어지거나
		Busy Waiting 하는 Thread 가 Core 보다 많으면 매우 비효율적이 된다.
	- Busy-Waiting 외의 방식
		- Block 방식
			- lock 을 획득하지 못하면 block 이 되어 CPU 가 다른 프로세스로 넘어간다.
	- 바쁜 대기를 전혀 사용하지 않을 수 있을까?
		- 바쁜 대기는 효율성의 문제이기 때문에 사용하지 않고 다른 lock 을 사용해도 문제가 생기지 않는다.
* 6.30 트랜잭션을 지원하는 로그 기반 시스템에서는 데이터항목에 대한 갱신은 해당 로그 레코드가 기록되기 전에는 실행될 수 없다.
이러한 제한이 필요한 이유를 설명하여라.
	- 만약 데이터 항목의 로그 레코드가 기록되기 전에 갱신을 하는 도중 시스템 장애로 데이터 항목을 복구해야 하는 경우,
	로그 레코드가 기록되지 않았으므로 원래의 값을 알 수가 없다.
		- 따라서 안전한 트랜잭션 연산을 위하여 로그 레코드를 먼저 기록해야 한다.
* 6.31 체크포인트 기법의 목적을 설명하라. 얼마나 자주 검사점이 실행되어야 할까?
다음과 같은 성능 요소에 체크포인트 실행의 빈도가 끼치는 영향을 설명하라.
	- 고장이 발생하지 않았을 경우의 시스템 성능
		- 고장이 발생하지 않았을 경우 체크포인트는 빈도가 많을수록 성능에 오버헤드만 증가한다.
	- 시스템 크래시로부터 복구하는 데 걸리는 시간
		- 전체 로그를 다 읽지 않고 마지막 체크포인트와 트랜잭션의 시작점부터 읽으면 되기 때문에 복구에 걸리는 시간이 훨씬 빨라진다.
	- 디스크 크래시로부터 복구하는 데 걸리는 시간
		- 디스크가 크래시가 났을 경우 체크포인트는 의미가 없는 거아닌가?
			- 손상된 부분이 어딘지 모르기 때문에 체크포인트 이전 이후 상관없이 전체를 검사해야 하는거 아닐까?
* 6.34 deposit( amount ) 와 withdraw( amount ) 의 두함수를 가진 은행 시스템을 고려하자.
이 두 함수는 은행계좌로 예금되거나 인출될 amount 를 전달받는다.
남편과 아내가 공유하는 은행 계좌가 있고 현재 남편은 withdraw() 함수를 아내는 deposit() 함수를 동시에 호출한다고 가정하자.
어떻게 경쟁 조건이 발생할 가능성이 있으며 경쟁 조건의 발생을 방지하기 위해 어떤 조취를 취할 수 있는지 기술하시오.
	- 경쟁 조건 발생 가능성
		- P1 : 계좌에 100 만원이 있을 때 남편이 withdraw() 함수를 사용하여 10만원을 인출하였고.
		- P2 : 계좌에서 10 만원이 차감되기 전에 아내가 10 만원을 deposit() 함수를 이용하여 넣음
		- P2 : 계좌는 100 + 10 을 해서 110 이 됨.
		- P1 : P1 은 P2 에서 값이 변한걸 모르기 때문에 100 만원에서 10 만원을 빼서 90 만원이 남음.
	- 경쟁 조건 발생 방지
		- withdraw() 함수와 deposit() 함수를 원자적으로 실행 또는 lock 을 이용하여 동시 실행을 방지.
