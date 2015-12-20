Chapter 03, Sharing data between threads

# Chapter 03, Sharing data between threads
* 이 장에서는 병행 처리에서 Thread 를 활용한 데이터 공유관계에서 생길 수 있는 문제점과 이를 방지하기 위한 방법에 대해 소개하고 있습니다.
- 병행 처리에서 Thread 는 Thread 사이에 쉽게 데이터 공유가 가능하므로 이점을 가지고 있다.
- Thread 사이에 데이터 공유에서 발생가능한 잠재적 문제를 피하고, 안전한 데이터 공유를 위해서 공유 규칙을 가져야 한다.


## 3.1 Problems with sharing data between threads
* *invarivant* 라는 단어의 정확한 정의가 나와있지 않지만 데이터베이스의 무결성과 비슷한 단어로 쓰입니다. 데이터의 정합성과 일관성을 유지할때 *invariant* 라 하며, 공유 데이터의 수정으로 인하여 다른 Thread 가 잘못된 데이터 정보를 보게 될 때 *broken invariant* 라 합니다.
- 모든 공유 데이터가 읽기 전용이라면 문제가 없지만, 하나이상의 쓰레드가 수정을 하려 한다면 잠재적인 위험이 존재한다.
    - *invariants*들은 종종 업데이트 동안 특히 만약 해당 자료구조가 복잡하거나 업데이트가 하나 이상의 값을 수정을 요할 때 깨지게 된다.
	- 쓰레드 사이에서 수정중인 데이터를 공유할 때 가장 간단한 문제는 *broken invariant* 이다.


### 3.1.1. Race conditions
* *race conditions* 는 경쟁상태라고 하며, 병렬 처리에서 서로 다른 Thread 가 공유자원에 동시에 접근할 때, 이로 인해 수행 결과를 예측할 수 없는 상태입니다.


### 3.1.2 Avoiding problematic race conditions
* *race conditions* 을 방지하는 방법
	- 보호 메커니즘을 추가하여, 오직 그 데이터를 수정하고 있는 Thread 만 베타적 접근을 허용하는 것.
	- *atomic* 연산을 이용하여 *lock-free programming* 을 사용하는 방법
	- 데이터베이스의 *transaction* 처럼 다루는 방법
		* *transaction* 이란 데이터의 수정 도중 다른 간섭이 생길 수 없는 *atomic* 한 연산으로 결과는 all or nothing 이다.  
		* 만약 여러 데이터의 수정이 한 *transaction* 으로 일어난다면, 결과는 모든 데이터의 수정이 성공하거나, 모든 데이터의 수정이 실패되어야 한다.


## 3.2 Protecting shared data with mutex
* mutex
	- 공유 데이터에 대한 접근 제어 방법
	- 상호배제적인 접근을 하도록 제어하여 한번에 하나의 Thread 만 접근이 가능하도록 한다. 따라서 한 Thread 가 데이터를 수정하는 동안 다른 Thread 가 *broken invariants* 를 보는 것이 불가능하게 된다.
	- *dead lock* 문제를 발생시킬 수 있다.

### 3.2.1 Using mutexes in C++
* C++에서 ```std::mutex```를 이용하면 mutex를 만들 수 있습니다.
	- 사용 가능한 함수
		- ```lock()```
		- ```unlock()```
		- ```trylock()```


#### Listing 3.1 Protecting a list with a mutex
```C++
#include <list>
#include <mutex>
#include <algorithm>

std::list<int> some_list;
std::mutex some_mutex;

void add_to_list(int new_value)
{
	std::lock_guard<std::mutex> guard(some_mutex);
	some_list.push_back(new_value);
}
bool list_contains(int value_to_find)
{
	std::lock_guard<std::mutex> guard(some_mutex);
	return std::find(some_list.begin(), some_list.end(), value_to_find)
		!= some_list.end();
}
```
* ```std::lock_guard``` 는 생성과 동시에 mutex 의 lock 을 획득하고, 객체가 소멸할 때 소멸자에서 unlock 이 호출 됩니다. 명시적으로 unlock 호출이 가능하지만, 이는 권장하지 않습니다.
  - std::mutex is usually not accessed directly: std::unique_lock and std::lock_guard are used to manage locking in exception-safe manner.
  - 사용자의 편의 또는 예외상황서 안전한 코드 동작을 위해 unlock 을 하지 않아도 소멸자가 unlock 을 하여 신뢰성 있는 코드를 만들어 준다.
* mutex 를 이용하여 데이터를 보호할 때, critical section 을 다루는 한 함수에서 보호된 데이터의 pointer나 reference를 리턴하면, 보호 메커니즘에 큰 문제가 생깁니다.
	- *해당 pointer나 reference에 접근하는 다른 코드들은 어떠한 제지도 없이 공유 데이터에 접근하여 데이터를 변경할 수 있습니다.*


### 3.2.2 Structuring code for protecting shared data
* 어떠한 멤버 함수도 보호되는 데이터의 pointer나 reference를 반환 값이나 인자로 보내지 않는이상, 해당 데이터는 안전합니다.
- pointer나 reference를 그들의 호출자에게 전달해주지 않는 멤버 함수를 검사하는 것 뿐만 아니라 당신의 제어 하에 있지 않은 함수에 pointer나 reference를 전달하는지 검사하는 것 또한 중요합니다.
	- 이는 매우 위험합니다. 이러한 함수들은 mutex의 보호 없이 추후에 사용될 수 있는 pointer나 reference를 저장할 수 있기 때문입니다.
	- 특히 이와 관련된 위험들은 다음의 리스트 처럼 함수 인자나 다른 방법을 통해 실행시간에 나올 수 있습니다.    

#### Listing 3.2 Accidentally passing out a reference to protected data
```C++
class some_data
{
	int a;
	std::string b;
public:
	void do_something();
};

class data_wrapper
{
private:
	some_data data;
	std::mutex m;
public:
	template<typename Function>
	void process_data(Function func)
	{
		std::lock_guard<std::mutex> l(m);
		func(data);	// Pass "protected" data to user-supplied function
	}
};

some_data* unprotected;

void malicious_function(some_data& protected_data)
{
	unprotected=&protected_data;
}

data_wrapper x;

void foo()
{
	x.process_data(malicious_function); // Pass in a malicious function
	unprotected->do_something(); //Unprotected access to protected data
}
```
- 이 예제에서 process_data 의 데이터를 lock_gaurd 를 이용하여 동시접근으로 부터 보호할 수 있습니다.
- 하지만 ```malicious_function``` 에서 공유 데이터의 포인터를 넘겨주기 때문에,
protected_data 에 보호 메커니즘 없이 접근을 하게 되어 공유 데이터를 보호할 수 없게 됩니다.


### 3.2.3 Spotting race conditions inherent in interfaces
- **listing 3.3**에서 보는 것 처럼 ```std::stack``` 컨테이너 어댑터와 같은 스택 자료구조가 있다고 가정해봅시다.

#### Listing 3.3 The interface to the ```std::stack``` container adapter
```C++
template<typename T, typename Container=std::deque<T> >
class stack
{
public:
	explicit stack(const Container&);
	explicit stack(Container&& = Container());
	template <class Alloc> explicit stack(const Alloc&);
	template <class Alloc> stack(const Container&, const Alloc&);
	template <class Alloc> stack(Container&&, const Alloc&);
	template <class Alloc> stack(stack&&, const Alloc&);

	bool empty() const;
	size_t size() const;
	T& top();
	T const& top() const;
	void push(T const&);
	void push(T&&);
	void pop();
	void swap(stack&&);
};
```

- std::stack``` 에 생성자와 swap 은 잠시 제쳐두고, 다음과 같은 5가지 멤버함수가 있습니다.
	- ```push()```를 이용해 새로운 요소를 스택에 추가.
	- ```pop()```을 이용해 요소를 스택에서 뺌.
	- ```top()```을 이용해 스택의 가장 맨위에 있는 요소를 읽음.
	- ```empty()```를 이용해 스택이 비었는지 확인.
	- ```size()```를 이용해 스택에 있는 요소의 갯수가 몇갠지 파악.
- ```top()```을 복사된 값을 반환하고, 이를 mutex를 이용해 내부 데이터를 보호하여도 race condition 발생 가능.
	- ```empty()```와 ```size()```는 여전히 다른 쓰레드들은 스택에 접근하는 것이 자유롭기 때문.
	- 특정 부분에서 ```stack``` 인스턴스가 만약 공유되지 않는다면, 다음과 같이 ```empty()```를 호출하여 확인하는 것이 안전하고, 만약 스택이 비어있지 않다면 ```top()```을 호출해야 한다.
	```C++
	stack<int> s;
	if(!s.empty())
	{
		int const value = s.top();
		s.pop();
		do_something(value);
	}
	```  
	- 이 코드는 Table 3.1 과 같이 멀티 쓰레드에서 안전하지 않습니다.
		- 스택 내용을 보호하기 위해 내부적으로 mutex를 사용하는 것은 이를 예방하지 못함
			- 이는 인터페이스의 결과이기 때문.
	- 해결책은 인터페이스를 바꾸는 것입니다.
		- 가장 간단한 경우로는 ```top()```이 호출 될 때 스택에 아무런 요소가 없으면 예외로 던지는 것.

#### Table 3.1 A possible ordering of operations on a stack from two threads
![Imgur](http://i.imgur.com/2eUjedZ.png)

- Table 3.1 에서 두개의 Thread A 와 B 의 *Value* 는 같은 값을 가질 수 있습니다.
	- 심지어 읽혀지지도 않은 하나의 값은 사라질 것 입니다.
	- 이는 또다른 Race Condition은 아니지만 ```empty()```/```top()``` 경쟁보다 더 문제가 될 수 있습니다.
- 따라서 인터페이스의 근본적인 변화가 필요하며, 
그 중 하나는 mutex의 보호 하에 있는 ```top()```과 ```pop()```을 결합시킨 것입니다.
- 하지만 이런 ```top()``` 과 ```pop()``` 의 결합은 또 다른 문제를 야기 시킵니다.
	- ```stack<vector<int>> 가 있다고 가정
		- 이 ```vector``` 는 동적 컨테이너고, vector 라이브러리에서 복사할 때, 힙으로부터 더 많은 메모리를 할당받아야 함.
		- 만약 ```pop()``` 함수가 꺼내진 값을 반환하고, 스택에서 해당 값을 제거한다면 정의되어 있다면
			- 꺼내지는 값은 스택이 수정된 후에 호출자에게 반환되지만, 만약 ```vector``` 생성자의 복사 생성자에 의해
			```std::bad_alloc``` 예외 처리가 된다면 스택에서 값은 제거되지만, 복사가 실패하므로
			꺼내진 데이터가 사라짐.
		- 따라서 ```std::stack``` 인터페이스는 ```top()``` 과 ```pop()``` 으로 나뉘어 디자인 됨.
		



	#### Option 1: Pass in a reference
	- 첫번 째 선택사항은 `pop()`을 호출한 인자로서 꺼내진 값을 받기 위한 변수를 reference로 넘겨주는 것입니다.
	```C++
	std::vector<int> result;
	some_stack.pop(result);
	```
	- 이는 많은 경우에서 잘 작동하지만, 타겟으로 이를 넘겨주기 위해 스택의 값의 타입에 대한 인스턴스의 생성자가 먼저 나와야 한다는 단점이 있습니다.
		- 인스턴스를 생성하는 것은 시간이나 자원의 측면에서 비싸기 때문에, 여기서 몇개의 타입은 쓸모가 없게됩니다.
		- 코드에서 이 관점에서 생성자가 필수적으로 사용가능하지 않은 파라미터를 요구하기 때문에, 여기서 다른 타입들은 항상 가능하지 않습니다.
		- 마지막으로, 저장된 형태가 할당 가능한 것을 요구합니다.
	- 이들은 중요한 제한입니다.
		- 많은 사용자 정의 형식은 할당을 지원하지 않지만, 이동 생성자나 복사 생성자(그러므로 값을 돌려주는 것을 할당하는) 것을 지원할지도 모릅니다.

	#### Opiton 2: Require a no-throw copy constructor or move constructor
	- 만약 값에 의해 반환된 것이 예외 처리될 수 있다면, 값을 반환하는 `pop()`과 관련된 예외 안전 문제가 오직 하나 있습니다.
		- 많은 타입은 예외 처리를 하지 않는 복사 생성자를 가지고 있고, C++ 표준에서 새로운 rvalue-reference 지원을 하여, 더 많은 타입은 비록 복사생성자가 예외처리를 하더라도 예외를 처리하지 않는 이동 생성자를 가지게 될 것입니다.
		- 한가지 가능한 선택사항은 예외 처리 없이 값에 의해 안전하게 반환될 수 있는 그러한 타입의 thread-safe 스택의 사용을 제한 하는 것입니다.
	- 비록 이는 안전하지만, 이상적이진 않습니다.
		- 심지어 `std::is_nothrow_copy_constructible`과 `std:is_nothrow_move_constructible` 형태 특징을 싸용하여 예외처리를 하지 않는 복사생성자나 이동생성자의 존재를 컴파일 시간에 확인할 지라도, 이는 꽤 제한적입니다.
		- 많은 예외처리를 하지만 이동생성자가 없는 복사생성자를 가지고 있는 사용자 정의 타입이 예외 처리를 할 수 없는 복사생성자와 이동생성자를 가지고 있는 것 보다 더 많습니다.
			- 비록 이는 C++11에서 rvalue-reference에 익숙해진 프로그래머들에겐 좀 약간의 차이는 있을 수 있습니다.
			- 이러한 타입들이 당신의 thread-safe stack에 저장될 수 없다면, 불행해질 겁니다.

	#### Option 3: Return a pointer to the popped item
	- 꺼내진 아이템의 값을 돌려주는 것보다 포인터를 돌려주는 것입니다.
		- 여기서의 이점은 예외 처리없이 자유롭게 포인터가 복사될 수 있어, Cargill의 예외 문제를 피할 수 있다는 것입니다.
		- 단점은 포인터를 반환하는 것이 객체에 할당 된 메모리 관리 수단과 정수같은 단순한 타입, 단순히 값을 돌려주는 비용을 초과할 수 있는 메모리 관리의 오버헤드를 요구한다는 것입니다.
		- 이 옵션을 사용하는 어떤 인터페이스들에겐, `std::shard_ptr`이 포인터 타입의 좋은 선택사항이 될 수 있습니다.
			- 객체의 마지막 포인터가 파괴되면 객체는 파괴되기 때문에 메모리 누수를 피할 수 있을 뿐만 아니라, 라이브러리가 메모리 할당 스킴의 모든 컨트롤을 하고 `new`와 `delete` 연산자를 사용할 필요가 없기 때문입니다.
			- 이는 최적화 목적에서 매우 중요할 수 있습니다.
				- `new` 연산자와 함께 할당된 스택에 있는 각각의 객체를 요구하는 것은 보통의 non-thread-safe 버전과 비교하여 상당한 오버헤드를 부과할 수 있습니다.

	#### Option 4: Provide both option 1 and either option 2 or 3
	- 제너릭 코드에서는 특히 유연성이 전혀 규정되어있지 않아야만 합니다.
	- 만약 옵션 2나 3을 선택했으면, 상대적으로 옵션 1을 제공하는 것 보다 쉽고, 이는 코드 사용자에게 가장 적은 비용을 들이면서 가장 적절한 어떤 옵션을 선택할 것인지를 제공합니다.

	#### Example definition of a thread-safe stack
	- **Listing 3.4**는 인터페이스에서 race condition이 없는 옵션 1과 3을 구현한 스택 클래스 정의를 보여주고 있습니다.

	#### Listing 3.4 An outline class definition for as thread-safe stack
	```C++
	#include <exception>
	#include <memory> // for std::shared_ptr<>

	struct empty_stack: std::exception
	{
		const char* what() const throw();
	};

	template<typename T>
	class threadsafe_stack
	{
	public:
		threadsafe_stack();
		threadsafe_stack(const threadsafe_stack&);
		// Assignment operator is deleted
		threadsafe_stack& operator=(const threadsafe-stack&) = delete;

		void push(T new_value);
		std::shared_ptr<T> pop();
		void pop(T& value);
		bool empty() const;
	};
	```

	- `pop()`에 대한 두가지 오버로드가 있고, 하나는 값을 저장한 위치에 대한 reference를 취하고 있으며, 다른 하나는 `std::shared_ptr<>`을 반환 합니다.
	- 이는 `push()`와 `pop()`을 가지는 단순한 인터페이스입니다.
	- 인터페이스를 쌍으로 만들면서 안전성을 최대화 할 수 있습니다.
		- 심지어 전체 스택에서의 동작이 제한적일지라도요.
		- 할당 연산자가 삭제되었기 때문에(appendix a, section A.2 참조),그리고 `swap()`함수가 없기 때문에 스택은 스스로 할당될 순 없습니다.
	- 그러나 복사가 될 수 있다면, 스택 요소들도 복사가 될 수 있다고 가정할 수 있습니다.
		- 만약 스택이 비어있다면, `pop()` 함수는 `empty_stack` 예외 처리를 할 수 있으며, 심지어 스택이 `empty()` 호출 이후에 수정이 되었을 때도 모든것이 그대로 작동할 수 있습니다.
	- 옵션 3에서 설명했듯이, `std::shared_ptr`의 사용은 스택이 메모리 할당 이슈를 잘 할 수 있게끔 하고, 원하는 경우 과도한 `new`와 `delete` 호출을 피할 수 있습니다.
		- 5개의 스택 연산이 `push()`, `pop()`, `empty()` 총 3개로 줄어들 수 있습니다.
			- 심지어 `empty()` 연산은 여분입니다.(잉여..)
	- 이러한 인터페이스의 단순함은 데이터에 대한 더 좋은 제어를 할 수 있게끔 합니다.
		- 한 연산의 전체에 대한 mutex lock을 보장할 수 있습니다.
	- 다음 리스트는 `std::stack<>`에 대한 간단한 구현을 보여줍니다.

	#### Listing 3.5 A fleshed-out class definition for a thread-safe stack
	```C++
	#include <exception>
	#include <memory>
	#include <mutex>
	#include <stack>

	struct empty_stack: std::exception
	{
		const char* what() const throw();
	};

	template<typename T>
	class threadsafe_stack
	{
	private:
		std::stack<T> data;
		mutable std::mutex m;
	public:
		threadsafe_stack(){}
		threadsafe_stack(const threadsafe_stack& other)
		{
			std::lock_guard<std::mutex> lock(other.m);
			data=other.data;	// Copy performed in consturctor body
		}
		threadsafe_stack& opterator=(const threadsafe_stack&) = delete;

		void push(T new_value)
		{
			std::lock_guard(std::mutex> lock(m);
			data.push(new_value);
		}
		std::shared_ptr<T> pop()
		{
			std::lock_guard<std::mutex> lock(m);
			if(data.empty()) throw empty_stack(); // Check for empty before trying to pop value
			//Allocate return value before modifying stack
			std::shared_ptr<T> const res(std::make_shared<T>(data.top()));
			data.pop();
			return res;
		}
		void pop(T& value)
		{
			std::lock_guard<std::mutex> lock(m);
			if(data.empty()) throw empty_stack();
			value=data.pop();
			data.pop();
		}
		bool empty() const
		{
			std::lock_guard<std::mutex> lock(m);
			return data.empty();
		}
	};
	```

	- 이 스택 구현은 사실 *복사가능합니다.*
		- 복사 생성자가 소스 객체안에서 mutex에 lock을 하고, 내부 스택에 복사합니다.
		- 복사를 할 때 mutex를 잡고 있는 것을 보장하기 위해 멤버 초기화 리스트 보다 생성자 안에서 복사를 합니다.
	- `top()`과 `pop`에 대한 논의에서 보여준 것 처럼, 인터페이스에서 문제있는 race condition은 너무 작은 단위를 lock을 하기 때문에 필연적으로 일어나게 됩니다.
		- 원하는 연산의 전체를 보호 하지 않습니다.
	- mutex와 관련된 문제는 너무 큰 단위를 lock 할 떄도 일어날 수 있습니다.
		- 극한의 상왕은 단일 전역 mutex가 모든 공유 데이터를 보호할 때 입니다.
		- 상당한 양의 공유 데이터를 다루는 시스템에서는, 이는 쓰레드가 해당 데이터의 다른 bit를 접근할 지라도 한번에 한 쓰레드만 동작하기 때문에 병행처리의 이점에 대한 성능을 없앨 수 있습니다.
	- 다중 프로세서 시스템을 다루기 위해 설계된 리눅스 커널의 초기버전은 단일 전역 커널 lock을 사용했었습니다.
		- 이는 작동하지만, 이(사)중 프로세서 시스템이 전형적으로 두(네)개의 단일 프로세서 시스템보다 더 안좋은 성능을 냄을 의미 했습니다.
	- 이 커널에서의 경쟁이 너무 많아서, 추가적인 프로세스 위에서 돌아가는 쓰레드들은 유용한 일을 하는 성능을 낼 수 없었습니다.
		- 이 리눅스 커널의 최근의 개정은 더 작은 단위의 lock scheme으로 옮겨갔고, 경쟁이 적어져 4중 프로세서 시스템은 단일 프로세서 시스템의 이상적인 4배에 가까운 성능을 내게 되었습니다.
	- 미세한 locking scheme의 한가지 이슈는 떄때로 한 연산에 있는 모든 데이터를 보호하기 위해 하나 이상의 mutex가 필요하다는 것입니다.
		- 이전에 봤던 것 처럼, 오직 하나의 mutex가 lock이 되야 할 필요가 있기 때문에, 떄떄로 mutex가 관장하는 데이터의 크기가 점점 커지는 경우가 있습니다.
			- 그러나 mutex들이 한 클래스의 각각의 인스턴스들을 보호하기 하는 경우처럼 의도하지 않은 경우도 있습니다.
			- 이러한 경우에는 다음 레벨에서의 lock은 사용자에게 lock을 내버려두고 단일 모든 클래스의 인스턴스를 보호하는 단일 뮤텍스를 가지는 것을 의미합니다.
				- 위의 두 경우 다 특별히 의도하진 않았습니다.
	- 만약 주어진 연산에 2개 이상의 mutex에 lock하는 것을 마쳤다면, 거기에는 *deadlock(교착상태)*라는 숨어있는 또 다른 잠재적인 문제가 있습니다.
		- 이는 race condition의 거의 반대입니다.
			- 2개의 쓰레드가 처음엔 경쟁을 했지만, 각 쓰레드는 다른 쓰레드를 기다리면서 둘 다 더이상 진행을 못하게 되는 경우지요.


### 3.2.4 Deadlock: the problem and a solution
* *deadlock* 은 교착상태라고도 하며 둘 이상의 프로세서가 서로 남이 가진 자원을 요구하면서, 
양쪽 모두 작업 수행을 할 수 없이 대기 상태로 놓여지는 상태를 말합니다.
	- Thread 1 이 자원 A 를 가진채로 자원 B 를 획득하고자 할 때 Thread 2 가 자원 B 를 가진채로 자원 A 를 획득하고자 한다면  
	( 각 자원이 베타적으로만 접근이 가능하다면 ) Thread 1 과 Thread 2 는 서로의 자원을 놓지 않은 채 획득하려고 하는 교착상태에 빠지게 됩니다.
	- 교착상태를 피하는 일반적인 방법은 항상 두개의 mutex를 같은 순서로 lock을 하는 것입니다.
		- 만약 mutex A를 mutex B보다 먼저 lock을 하면, 절대 교착상태가 발생하지 않습니다.
	- 같은 클래스의 두 인스턴스 사이에 데이터를 swap 하려 할 때 데이터의 정확성을 보장하기 위해선 mutex 가 필요.
		- swap 을 위해서 각각의 인스턴스에 lock 을 건다. 그런데 데이터가 들어오는 순서는 항상 바뀔 수 있으므로 deadlock 이 발생할 수 있다.
			- ```std::lock``` 
				- 상기의 교착 상태의 위험 없이 2개 이상의 mutex에 lock을 걸 수 있다.


#### Listing 3.6 Using `std::lock()` and `std::lock_guard` in a swap operation
```C++
class some_big_object;
void swap(some_big_object& lhs, some_big_object& rhs);

class X
{
private:
	some_big_object some_detail;
	std::mutex m;
public:
	X(some_big_object const& sd):some_detail(sd){}

	friend void swap(X& lhs, X& rhs)
	{
		if (&lhs==&rhs)
			return;
		std::lock(lhs.m, rhs.m); // (1)
		std::lock_guard<std::mutex> lock_a(lhs.m, std::adopt_lock); // (2)
		std::lock_guard<std::mutex> lock_b(rhs.m, std::adopt_lock); // (3)
		swap(lhs.some_detail, rhs.some_detail);
	}
};
```

- 첫째로 `std::mutex`를 이미 정의되지 않은 행동으로 잡고 있을 때 `std::mutex`에 lock을 얻기위해 시도하기 때문에, 인자들이 서로 다른 것임을 보장하기 위해 확인 됩니다.
	- 같은 쓰레드에서 여러 lock을 허락한 mutex는 `std::recursive_mutex`의 형태로 제공됩니다. 3.3.3절에서 자세히 보실 수 있습니다.
- 그리고 `std::lock()` - (1) 을 호출해 두 mutex에 lock을 걸고, 두 `std::lock_guard`-(2),(3) 각 생성자를 호출하여 인스턴스가 생성됩니다.
	- `std::adopt_lock` 파라미터가 mutex가 이미 lock을 걸어놓은 `std::lock_guard`를 가리키기 위해 추가적인 mutex를 보급되었고, 생성자에 있는 mutex에 lock을 거려고 시도하는 것 보다 이미 mutex에 존재하는 lock의 소유권을 차용해야 합니다.
- 이는 mutex가 보호 연산이 예외처리 된 일반적인 경우에서 정확하게 함수에서 unlock되었음을 보장합니다.
	- 이는 또한 간단한 반환을 허용합니다.
- 또 이는 `std::lock`이 예외 처리할 수 있는 호출 내부에서 lhs.m이나 rhs.m에 lock을 하는 것에 주목할 필요가 없습니다.
	- 이 경우에는 예외가 `std::lock`에 의해 전달됩니다.
- 만약 `std::lock`이 성공적으로 하나의 mutex의 lock을 얻었으면 다른 mutex의 lock을 얻으려고 시도할 때 마다 예외처리가 된다면, 이 첫번째 lock은 자동적으로 해제됩니다.
	- `std::lock`은 mutex들에 lock을 걸 때 전부 아니면 아무것도 아닌 방식을 제공합니다.
- 비록 `std::lock`이 두개 이상의 lock을 함께 얻을 경우에서 교착상태를 피할 수 있도록 도움을 줄 지라도, 그들이 각각 얻어졌을 때는 도움을 주지 않습니다.
	- 이러한 경우에는 교착상태가 발생하지 않음을 개발자로서의 훈련을 통해 해결해야만 합니다.
	- 이는 쉽지 않습니다.
		- 교착상태는 멀티쓰레드 코드에서 어려운 문제중 하나고, 대부분의 시간에서 모든것이 제대로 작동하는데도 종종 예측 불가능 합니다.
		- 그러나 deadlock-free 코드를 작성하는데 도움을 줄 수 있는 비교적 간단한 규칙이 있습니다.

### 3.2.5 Further guidlines for avoiding deadlock
- 교착상태는 lock 과 함께 일어나는 것은 아닙니다. 비록 대부분의 경우는 lock과 함께 일어나지만요.
	- 두 개의 쓰레드와 `std::thread` 객체를 서로 다른 객체에 `join()`을 하게 하면 lock 없이 교착상태를 만들 수 있습니다.
	- 이 경우에는 어떤 쓰레드도 서로가 마치기를 기다리기 때문에 더이상 진행할 수 없습니다.
- 이 단순한 사이클은 다른 쓰레드가 동시에 첫번째 쓰레드를 기다릴 수 있다면 한 쓰레드가 다른 쓰레드를 위해 몇가지 액션을 하기위해 기다리는 어디서든 일어날 수 있습니다.
- 그리고 이는 두 쓰레드에만 제한된 것이 아닙니다.
	- 세개 이상의 쓰레드 또한 여전히 교착상태를 야기할 수 있습니다.
- 교착상태를 피하는 가이드라인은 하나의 아이디어에서 나왔습니다.
	- 만약 당신이 기다리고 있던 찬스가 생겼다면 다른 쓰레드를 기다리지 말라는 것입니다.
	- 독립적인 가이드라인은 다른 쓰레드가 당신을 기다리는 가능성을 파악하고 제거하는 방법을 제공합니다.

#### Avoid nested locks
- 첫 번째 아이디어는 가장 간단합니다.
	- 이미 하나를 잡고 있으면 lock을 얻지 않는 것입니다.
	- 만약 이 가이드라인을 따른다면, 각 쓰레드는 오직 하나의 단일 lock만 잡기 때문에 lock을 홀로 사용하여 교착상태가 일어나는 것이 불가능 합니다.
	- 다른 것들(쓰레드가 서로를 기다리는 것)에서는 여전히 교착상태에 빠질 수 있지만, mutex lock은 대부분의 교착상태의 이유입니다.
	- 만약 다중 lock이 필요하다면 교착 상태 없이 다중 lock을 얻기 위해 `std::lock`과 함께 단일 lock처럼 하세요.

#### Avoid calling user-supplied code while holding a lock
- 이 가이드는 이전의 가이드라인의 후속입니다.
	- 사용자가 작성한 코드이기 때문에, 이 코드가 무엇을 할 수 있는지 알 수 없습니다.
		- lock을 얻는 것을 포함한 모든 것을 할 수 있는 코드입니다.
	- 만약 사용자가 작성한 lock을 잡고있는 코드를 호출하고 이 코드가 lock을 얻는다면, 중첩된 lock을 피하는 가이드라인을 파괴하고 교착상태에 빠질 수 있을 것입니다.
- 때때로 이는 피할 수 없으며, 만약 3.2.3절의 스택처럼 제너릭 코드를 작성한다면, 파라미터 형에 대한 모든 연산은 사용자 작성 코드입니다.
	- 이 경우에는 새로운 가이드 라인이 필요합니다.

#### Acquire locks in a fixed order
- 만약 절대적으로 2개 이상의 lock을 얻어야 한다면, `std::lock`과 함께 단일 연산으로 그들을 얻을 수 없을 것입니다.
	- 그 다음으로 좋은 것은 매 쓰레드에서 같은 순서로 lock을 얻는 것입니다.
- 이는 3.2.4절에서 두개의 mutex를 얻을 때 교착상태를 피하는 한 방법으로 소개를 했습니다.
	- 핵심은 쓰레드사이의 일관성있는 순서를 유지하는 것입니다.
	- 몇가지 경우에서 이는 비교적 쉽습니다.
		- 예를 들면, 3.2.3절에 있는 스택을 보시면 mutex는 사용자 지정 코드를 호출하는 스택안에 저장된 자료의 연산이 아닌 각 스택 인스턴스의 내부에 있습니다.
		- 그러나, 스택 안에 저장된 자료의 연산중 어떤것들도 스택 자체에서 연산을 수행해야 하는것을 제약하는 제약조건을 추가할 수 있습니다.
		- 이는 스택의 사용자에게 부담이 되지만, 해당 컨테이너를 접근하기위해 컨테이너에 저장된 데이터를 위한것보다 더 일반적이지 않으며, 이러한 일이 일어날 때 꽤 명백합니다.
			- 그래서 이것은 옮기는데 특별히 어려운 부담이 되진 않습니다.
- 3.2.4절 에서 보았던 swap 연산 처럼 다른 경우에선 이는 그렇게 직관적이진 않습니다.
	- 적어도 이 케이스에서는 mutex를 동시에 lock을 할 수 있을것이지만, 항상 가능한 것은 아닙니다.
	- 3.1절의 linked list 예제로 돌아가서, 리스트를 보호할 수 있는 하나는 노드 당 mutex를 하나씩 가지는 것임을 볼 수 있을 것입니다.
	- 그리고나서 리스트에 접근하기 위해서, 쓰레드는 반드시 그들이 접근하고자 하는 모든 노드에 lock을 얻어야만 합니다.
	- 아이템을 삭제하기 위한 쓰레드는, 세개의 노드에 lock을 얻어야만 합니다.
		- 앞에서 보았던 수정하는 방법처럼 삭제되는 노드와 양 사이드에 있는 노드를 세개를 얻어야합니다.
		- 마찬가지로 리스트를 순회하기 위해서 동시에 수정되지 않은 다음 포인터를 보장하기 위해서 반드시 다음 순서에 있는 lock을 얻는동안 현재 노드의 lock을 유지해야합니다.
		-  한번 다음 노드의 lock을 얻으면, 첫 번째 lock은 더이상 필요하지 않기 때문에 해제됩니다.
	- 이 hand-over-hand lock 스타일은 다중 쓰레드가 다른 노드가 각각 접근하도로 제공된 리스트에 접근하는 것을 허용합니다.
	- 그러나 교착상태를 방지하기 위해, 노드는 항상 반드시 동일한 순서로 lock이 되어야합니다.
		- 만약 두 쓰레드가 hand-over-hand 방식으로 리스트를 역순으로 순회하려고 한다면, 이들은 리스트의 중간에서 각각 서로에게 교착상태를 걸 수 있습니다.
		- 노드 A와 노드 B가 리스트에서 인접한다면, 쓰레드는 노드 A에 lock을 잡으려고 시도할 것이고, 노드 B에 lock을 얻으려고 시도할 것입니다.
		- 다른 방향으로 가는 쓰레드가 노드 B의 lock을 잡고 있고 노드 A에 lock 얻으려고 한다면, 교착상태의 전형적인 시나리오 입니다.
	- 마찬가지로 노드 A와 C 사이에 있는 노드 B를 삭제 할 때, 쓰레드가 A와 C의 lock을 얻기전에 B의 lock을 얻는다면, 리스트순회 쓰레드와 교착상태가 일어날 수 있습니다.
	- 그러한 쓰레드는 A나 C 둘다 처음에 lock을 걸려고 시도하지만(순회 방향에 따라) lock B를 얻을 수 없나는 것을 알게 됩니다.
		- 왜냐하면 삭제하는 작업을 하는 쓰레드가 lock B를 잡고 있고 A와 C를 lock을 얻으려고 시도하고 있기 때문입니다.
	- 교착상태를 방지하는 한 방법은 순회 순서를 결정하고 항상 반드시 lock C를 얻기전에 B를 얻고, B를 얻기전에 A를 얻어야만 합니다.
	- 이는 허용하지 않는 역방향 순회의 비용에서 교착상태의 가능성을 제거할 것입니다.
	- 유사한 규칙은 종종 다른 자료구조에 확립될 수 있습니다.

#### Use a lock hierarchy
- 비록 이는 lock 순서를 결정하는 정말  특별한 경우일지라도, lock 계층은 규칙이 실행시간에 적용되는 것을 확인하는 방법을 제공합니다.
	- 아이디어는 응용 프로그램을 층으로 나누고 주어진 층에 lock이 될 수 있는 모든 mutex를 파악하는 것입니다.
	- 코드가 mutex에 lock을 걸려고 시도할 때마다, 만약 이미 낮은 층에서 lock을 잡고 있다면 mutex에 lock을 허용하지 않습니다.
	- 각 mutex에 층 번호를 할당하고 각 쓰레드에 어떤 mutex가 lock이 되었는지에 대한 기록을 유지하면서 실행시간에 이를 확인할 수 있습니다.
	- 아래의 리스트는 계층화 된 mutex를 두개의 쓰레드가 사용하는 예를 보여줍니다.

#### Listing 3.7 Using a lock hierarchy to prevent deadlock
```C++
hierarchical_mutex high_level_mutex(10000);	// (1)
hierarchical_mutex low_level_mutex(50000);	// (2)

int do_low-level_stuff();

int low_level_func()
{
	std::lock_guard<hierarchical_mutex> lk(low_level_mutex);	// (3)
	return do_low_level_stuff();
}

void high_level_stuff(int some_param);

void high_level_func()
{
	std::lock_guard<hirearchical_mutex> lk(high_level_mutex);	// (4)
	high_level_stuff(love_level_func());	// (5)
}

void thread_a()	// (6)
{
	high_level_func();
}

hierarchical_mutex other_mutex(100);	// (7)
void do_other_stuff();

void other_stuff()
{
	high_level_func();	// (8)
	do_other_stuff();
}

void thread_b()	// (9)
{
	std::lock_guard<hierarchical_mutex> lk(other_mutex);	// (10)
	other_stuff();
}
```

- `thread_a()` 는 룰을 준수하므로, 제대로 돌아갑니다. 반면에 `thread_b()`는 룰을 따르지 않으므로 실행 시간에 실패할 것입니다.
	- `thread a()`는 `high_level_mutex`(계층 값이 10000)를 lock을 하는 `high_level_func()`을 호출하고,
	- `high_level_stuff()`를 위해 파라미터를 얻기위해 lock을 건 이 mutex와 함께 `low_level_func()`를 호출합니다.
	- 그리고 `low_level_mutex`를 lock을 하지만 이 mutex는 5000의 계층값을 가지므로 괜찮습니다.
-  반면에, `thread_b()`는 좋지 않습니다.
	- 처음에 이 쓰레드는 고작 100의 계층 값을 가지고 있는 `other_mutex`에 lock을 겁니다.
		- 이는 정말로 엄청나게 낮은 레벨의 데이터를 보호해야만 한다는 것을 의미합니다.
	- `other_stuff()`가 `high_level_func()`를 호출할 때, `high_level_func()`는 현재 100이라는 계층값보다 상당히 큰 10000이라는 계층값을 가진 `high_level_mutex`를 얻으려고 시도하므로 계층이 망가지게 됩니다.
	- `hierarchical_mutex`는 프로그램의 비정상 종료나 에외처리와 같은 에러를 보고하게 될 것입니다.
- mutex가 스스로 서로의 lock 순서를 강요하기 때문에 계층구조의 mutex 사이에서의 교착상태는 불가능하게 됩니다.
	- 이는 계층의 같은 레벨에서 두개의 lock 을 동시에 잡을 수 없으며, hand-over-hand locking scheme은 몇가지 경우에서는 불가능할지도 모르는 이전보다 낮은 값을 가지는 사슬에 있는 mutex를 요구하게 됩니다.  
- 이 예는 또한 사용자 정의의 mutex 타입와 함께 `std::lock_guard<>` 템플릿의 사용법을 보여줍니다.
- `hierarchical_mutex`는 표준이 아니지만 작성하기 쉽습니다.
	- listing 3.8에서 단순한 구현을 보여줍니다.
- 비록 사용자 정의 타입일지라도, `lock()`과 `unlock()`, 그리고 `try_lock()`라는 mutex 개념을 만족시키기 위해 필요한 세가지 멤버 함수를 구현해놓았기 때문에 `std::lock_guard<>`와 함께 사용될 수 있습니다.
	- 아직 직접 `try_lock()`의 사용을 보지 못하셨지만, 꽤 간단합니다.
		- mutex의 lock을 다른 쓰레드가 잡고 있으면, 호출한 쓰레드가 해당 mutex의 lock을 얻을 때까지 기다리는 것 보다 `false`를 반환하는 것입니다.
		- 이는 또한 내부적으로 교착상태 회피 알고리즘의 한 부분처럼 `std::lock()`에 의해 사용됩니다.

#### Listing 3.8 A simple hierarchical mutex
```C++
class hierarchical_mutex
{
	std::mutex internal_mutex;
	unsigned long const hierarchy_value;
	unsigned long previous_hierarchy_value;
	static thread_local unsigned long this_thread_hierarchy_vale;	// (1)

	void check_for_hierarchy_violation()
	{
		if(this_thread_hierarchy_value <= hierarchy_value)	// (2)
		{
			throw std::logic_error("mutex hierarchy violated");
		}
	}
	void update_hierarchy_value()
	{
		previous_hierarchy_value=this_thread_hierarchy_value;	// (3)
		this_thread_hiearchy_value=hierarchy_value;
	}
public:
	explicit hierarchical_mutex(unsigned long value);
		hierarchy_value(value),
		previous_hierarchy_value(0)
	{}

	void lock()
	{
		check_for_hierarchy_violation();
		internal_mutex.lock();	// (4)
		update_hierarchy_value();	// (5)
	}
	void unlock()
	{
		this_thread_hierarchy_value=previous_hierarchy_value;	// (6)
		internal_mutex.unlock();
	}
	bool try_lock()
	{
		check_for_hierarchy_violation();
		if(!internal_mutex.try_lock())	// (7)
			return false;
		update_hierarchy_value();
		return true;
	}
};
thread_local unsigned long
	hierarchical_mutex::this_thread_hierarchy_value(ULONG_MAX);	// (8)
```

- 여기서 핵심은 현재의 쓰레드가 가지고 있는 계층값`this_thread_hierarchy_value`-(1) 을 표현하기 위한 `thread_local` 값의 사용입니다.
	- 가장 큰 값-(8)로 초기화가 되었고, 어떤 mutex들도 lock이 될 수 있습니다.
		- 왜냐하면 `thread_local`로 선언이 되었기 때문에, 모든 쓰레드가 소유권을 복사할 수 있으며, 한 쓰레드의 상태가 다른 쓰레드로부터 읽힐 때 변수의 상태는 전부다 독립적이게 됩니다.
		- `thread_local`에 대한 더 자세한 내용을 보시려면 appendix A, A.8절을 참조하세요.
- 그래서 처음에 쓰레드가 `ULONG_MAX`값을 가지는 `this_thread_hierarchy_value`값으로 `hierarchical_mutex` 인스턴스를 lock을 겁니다.
	- 자연스럽게 이는 다른 값보다 큰 값을 갖게 되며, `check_for_hierarchy_violation()`-(2) 검사를 통과하게 됩니다.
	- 이 방법을 통과하면 `lock()`은 실제로 내부 mutex에 lock을 걸게 됩니다.-(4)
	- 한번 이 lock이 성공하면, 계층값을 업데이트 할 수 있습니다.-(5)        
- 만약 처음 lock을 잡고 있는동안 *또 다른* `hierarchical_mutex`에 lock을 걸고자 한다면, `this_thread_hierarchy_value`의 값은 첫 번째 mutex의 계층 값을 반영합니다.
	- 이 두 번째 mutex의 계층 값은 (2)를 통과하기 위해서 이미 lock이 걸려있는 지금의 mutex의 값보다 반드시 작아야만 합니다.
- 이제 현재 쓰레드를 위해 이전의 계층값을 저장하는 것이 중요하여, `unlock()`-(6)을 통해 저장할 수 있습니다.
	- 반면에 어떤 쓰레드도 lock잡고 있지 않더라도 더 높은 값으로 다시 mutex에 lock을 거는것은 불가능합니다.
	- 왜냐하면 이 이전의 계층값은 `intenal_mutex`-(3)를 잡고 있을 때만 저장할 수 있기 때문에, 내부의 mutex를 해제하기 *전에*-(6) 저장해야 하며 이것은
	  내부의 mutex의 lock에 의해 안전하게 보호되고 있기 때문에 `hiearchical-mutex` 자신에게 이를 안전하게 저장할 수 있습니다.   
- `try_lock()`은 만약 `internal_mutex`-(7)에서 `try_lock()` 호출이 실패한다면, lock을 소유하지 않고, 그래서 계층값을 갱신하지 못하며, `true`대신 `false`를 반환한다는 것을 제외하고는 `lock()`과 똑같습니다.
- 비록 검사가 실행시간에 확인일 지라도, 적어도 시간에 의존적이진 않습니다.
	- 교착상태를 발생하는 극히 드문 조건을 기다려야 하지는 않습니다.
- 또한 이 방법에서 응용 프로그램이랑 mutex로 나뉘어지도록 요구된 설계 프로세스가 그들이 쓰여지기 전에 교착상태를 야기할 수 있는 많은 가능성을 제거하는데 도움을 줄 수 있습니다.
	- 이는 실제로 실행시간 확인을 쓰지 않더라도 설계 행동을 하는데 가치가 있을 수 있습니다.

#### Extending these guidelines beyond locks
- 이번 절의 처음에 말했던 것 처럼, 교착상태는 단지 lock과 함께 일어나지는 않습니다.
	- 대기 사이클을 야기하는 어떠한 동기화 생성에서도 일어날 수 있습니다.
- 그러므로 이러한 경우를 다룰 수 있게 하기 위해 이 가이드라인을 확장하는 것은 가치가 있습니다.
- 예를 들면 가능하다면 중첩된 lock을 얻는 것을 피애햐 하지만, lock을 잡고있는 쓰레드를 기다리는 것은 좋지 않은 아이디어 입니다.
	- 해당 쓰레드가 진행하기 위해서 해당 lock을 얻을 필요도 있으니깐요.
- 유사하게 끝나길 기다리는 쓰레드를 기다린다고 하면, 해당 쓰레드가 낮은 계층의 쓰레드를 기다리는 것과 같은 쓰레드 계층을 확인하는 것은 가치가 있습니다.
	- 이를 하기 위한 한 단순한 방법으로는 3.1.2절과 3.3절에서 써놓은 것 처럼 쓰레드를 시작하는 같은 함수에서 함께 하는 것을 보장하는 것입니다.
- 교착상태를 피하는 코드를 설계했다면, `std::lock()`과 `std::lock_guard`는 대부분의 단순한 lock의 경우를 다루지만, 떄때로는 더 유연함도 요구됩니다.
- 그러한 경우에는 표준 라이브러리가 `std::unique_lock` 템플릿을 제공합니다.
	- `std::lock_guard`처럼, 이 클레스 템플릿은 mutex타입을 파라미터화 하였고, 같은 RAII-style lock 관리 기법을 좀 더 유연하게 `std::lock_guard`처럼 제공합니다.     	 


### 3.2.6 Flexible locking with std::unique_lock
* ```std::unique_lock``` 은 mutex 소유권 이전을 허용함으로서 좀더 유연한 mutex 의 활용이 가능합니다.
  - ```std::unique_lock```  의 두번째 인자는 다음과 같습니다.
    - ```std::adopt_lock``` : mutex 가 lock 상태로 생성되며 lock 객체가 mutex 의 lock 을 관리합니다,
    - ```std::defer_lock``` : mutex 를 unlocked 상태로 생성합니다.  
      - lock 을 획득하려면 ```std::unique_lock``` 객체( mutex 가 아닌 )의 ```lock()``` 을 호출하거나 ```std::unique_lock``` 객체를 ```std::lock()``` 에 인자로 전달함으로서 lock 을 획득할 수 있습니다.  
- Listing 3.6 의 ```std::lock_guard``` 와 ```std::adopt_lock``` 을 ```std::unique_lock``` 와 ```std::defer_lock``` 로 대체하면 Listing 3.9 와 같이 쉽게 쓰일 수 있습니다.
- ```std::unique_lock``` 은 mutex 의 소유권 정보를 저장하고 업데이트 해야하기 때문에, ```std::lock_guard``` 보다 더 많은 공간을 필요로 하고 부분적으로 보다 느립니다.


##### Listing 3.9 Using std::lock() and std::unique_lock in a swap operation
```c++
class some_big_object;
void swap(some_big_object& lhs,some_big_object& rhs);

class X
{
	private:
		some_big_object some_detail;
		std::mutex m;
	public:
		X(some_big_object const& sd):some_detail(sd){}

		friend void swap(X& lhs, X& rhs)
		{
			if(&lhs == &rhs)
				return;

			std::unique_lock<std::mutex> lock_a(lhs.m,std::defer_lock); /* (1) */
			std::unique_lock<std::mutex> lock_b(rhs.m,std::defer_lock); /* (1) */

			std::lock(lock_a,lock_b); /* (2) */
			swap(lhs.some_detail,rhs.some_detail);
		}
};
```

- ```std::unique_lock``` 는 ```lock()```, ```try_lock()``` 그리고 ```unlock()``` 멤버 함수를 지원합니다
	- 이 멤버함수들은 ```std::unique_lock``` 인스턴스 내부의 flag 를 바로 갱신합니다.
		- flag 는 현재 인스턴스의 mutex 소유 여부를 나타냅니다.
		- flag 는 ```owns_lock()``` 멤버 함수를 호출하여 조회할 수 있습니다.
	- 이 flag 정보를 저장하고 갱신해야 하기 때문에, 일반적으로 ```std::unique_lock``` 객체의 크기는 ```std::lock_guard``` 객체보다 크며, 약간의 성능상 페널티가 생깁니다
- 이 예제는 앞에서 이미 보았던 *deferred locking* 입니다; 또 다른 케이스는 lock 에 대한 소유권이 다른 scope 로 이동하는 예입니다.


### 3.2.7 Transferring mutex ownership between scopes

- ```std::unique_lock``` 인스턴스는 mutex 의 소유권을 인스턴스 사이에 이동을 통해 전달 가능합니다.
- 소유권 전달은 인스턴스의 리턴을 통해 자동적으로 전달되거나, 명시적으로 ```std::move()``` 함수 호출을 통해 이루어집니다.  
	- 기본적으로 이런 소유권 전달은 source 의 종류에 따라 정해집니다.
		1. *lvalue*( 실제 값 또는 참조자 )
			- 변수로부터 소유권이 의도치 않게 전달 되는 것을 피하기 위해 명시적으로 소유권을 전달해야 합니다.
		2. *rvalue*( 임시적인 값의 한종류 )
			- 자동적으로 소유권이 전달됩니다.
- ```std::unique_lock``` 은 이동가능하지만 복사 불가능한 타입 중 하나입니다.   
- 아래의 코드는 이러한 예제 중 하나입니다.


```c++
std::unique_lock<std::mutex> get_lock()
{
	extern std::mutex some_mutex;

	std::unique_lock<std::mutex> lk(some_mutex);
	prepare_data();

	return lk; /* (1) */
}
void process_data()
{
	std::unique_lock<std::mutex> lk(get_lock()); /* (1) */
	do_something();
}
```
- lk 는 함수안에서 *automatic* 변수로 선언되었기 때문에, 이것은 ```std:move()``` 없이 직접적으로 반환 가능합니다;
	- 컴파일러는 *move constructor* 호출을 담당합니다.


- ```process_data()``` 함수는 (2) 의 ```std::unique_lock``` 인스턴스가 소유하고 있던 소유권을 직접적으로 전달 받을 수 있습니다,
그리고 호출되는 ```do_somthing()``` 함수는 작업 시간 동안 다른 스레드로 인해 데이터가 변질 되지 않을 것이라고 신뢰 할 수 있습니다.
	- 전형적으로 이런 mutex 의 lock 획득 위치 선정 문제는,
	현재 프로그램의 상태 또는 ```std::unique_lock``` 객체를 반환하는 함수의 전달 인자에 따릅 결정됩니다.
- lock의 소유권 획득에 관한 방법 중 하나는 *gateway class* 입니다.
	- *gateway class* 는 직접적으로 lock 을 리턴하지는 않지만,
	이 클래스의 멤버가 보호된 데이터로의 접근에 대한 lock 이 올바르다는 것을 보장합니다.
- 이 케이스에서, 데이터로의 모든 접근은 이 *gateway class* 를 통합니다.
	- 데이터에 접근하기를 원할 때,
	락을 걸 수 있는 *gateway class* ( 앞선 예제에서 get_lock() 과 같은 함수 ) 의 객체를 획득 합니다.
	그러면 *gateway* 객체의 멤버 함수를 통해 데이터에 접근할 수 있습니다.  
	- 작업이 끝나면, *gateway* 객체를 파괴하여,
	락을 해제하고, 다른 스레드가 보호 데이터에 접근을 할 수 있도록 해야 합니다.  
	- 이 *gateway* 객체는 이동가능하고 ( 때문에 함수에 의해 반환 가능합니다 ),
	이런 상황에선 lock 객체 데이터 멤버 역시 이동 가능해야 합니다.
- ```std::unique_lock``` 은 인스턴스가 파괴되기 전에 그들의 lock 을 양도 하는 것을 허용하기 때문에, mutex 처럼 ```unlock()``` 멤버 함수를 이용해 할 수 있습니다.
	- ```std::unique_lock``` 은 mutex 와 같은 기본적인 locking 과 unlocking 멤버 함수 기능을 제공하여, ```std::lock``` 과 같이 generic functions 과 같이 사용 가능합니다.
- ```std::unique_lock``` 객체가 파괴되기전 lock 의 해제가 가능 하다는 건,
	특정한 코드 branch 에서 lock 이 명백하게 필요 없어 졌을 때 lock 을 선택적으로 해제할 수 있음을 의미합니다.
	- 이것은 애플리케이션의 성능에 매우 중요한 요소가 됩니다.
		- 필요 이상으로 lock 을 잡고 있으면, lock 을 대기하는 다른 스레드가 필요이상으로 오래 진행을 방해 받기 때문입니다.


### 3.2.8 Locking at an appropriate granularity

- lock 의 *granularity* 는 이전 섹션 3.2.3 다뤘습니다 :
	- lock 의 *granularity* 는 single lock 에 의해 보호되는 데이터의 양을 설명하기 위한 *hand-waving* 용어입니다.
- 세밀한 lock 은 작은 양의 데이터를 보호하고, 대단위 lock 은 많은 양의 데이터를 보호합니다.
- 대단위 lock 의 선택을 할 때에는, 데이터 보호됨을 보장 하는것 뿐 아니라,
 lock 이 실질적으로 필요할 때만 동작하는 것을 보장하는 일 또한 매우 중요한 과제입니다.


- 우리는 일상 생활에서 다음과 같은 매우 짜증 스러운 상황을 본 적이 있을 것 입니다.  
	- 당신은 슈퍼마켓에서 야채로 꽉찬 카트를 가지고 계산을 위해 줄서서 기다리고 있습니다.
	그런데 이 때 계산을 받고 있던 사람이 갑자기 크랜배리 소스를 빠뜨린 것을 깨닫고는 기다리는 모두를 뒤로한채 그것을 찾으러 가버렸습니다.
	또는 계산원은 계산할 준비를 이제 막 하고 있고 손님은 계산대 앞에서 지갑을 찾기 시작했습니다.
	만약 이 때 모든 사람이 사려고 한 물건을 이미 다 준비하고,
	적절한 지불 수단을 미리 준비한 상태에서 체크아웃을 한다면 모든 작업 진행이 더 쉬워질 것 입니다.


- 이와 같은 일은 스레드에도 적용됩니다.  
	- 만약 다중 스레드가 같은 자원을 기다리고 있다면 ( 계산하는 직원 ),
	그리고 어떤 스레드가 필요 이상으로 lock 을 획득 하고 있다면,
	이는 전체 대기 시간을 증가시킬 것 입니다. ( 당신이 계산대에 도착해서 크랜베리 소스를 찾기 전까진 기다리지 않습니다. )
- 한가지 가능한 해결 방법은, 공유 데이터를 실제로 접근할때만 mutex 를 lock 하는 것입니다;
	- 파일 I/O 와 같은 시간을 오래 지체하는 작업은 lock 을 획득한 상태에서 하지 말아야 합니다.
	파일 I/O 는 전형적으로 메모리에서 같은 양의 데이터를 읽거나 쓰는 것보다 수백배 ( 또는 수천 ) 느립니다.
	그렇기 때문에 만약 파일로의 접근을 막는게 정말로 의도한게 아니라면,
	lock 을 획득한 채 I/O 를 수행하는 것은 다른 스레드들을 불필요한 지연을 겪게 할 것 입니다 ( 왜냐하면 다른 스레드들은 그동안 lock 을 얻기위해 대기할 것 입니다. ),
	또한 다중 스레드의 이점 역시 사라질 것 입니다.
- ```std::unique_lock``` 는 이런 상황에 유용합니다. 왜냐하면 코드가 더이상 공유 데이터에 접근할 필요가 없을 때 ```unlock()``` 을 호출할 수 있고,
	다시 접근이 필요할 때 코드에서 다시 ```lock()``` 을 호출할 수 있기 때문입니다.


```c++
void get_and_process_data()
{
	std::unique_lock<std::mutex> my_lock(the_mutex);
	some_class data_to_process = get_next_data_chunk();
	my_lock.unlock(); /* (1) */

	result_type result = process(data_to_process);

	my_lock.lock(); /* (2) */
	write_result(data_to_process,result);
}
```


- ```process()``` 을 호출할 때 mutex 를 lock 할 필요가 없습니다.
따라서 ```process()``` 를 호출하기 전에 (1) lock 을 수동적으로 해제해야 하고 lock 은 ```process()``` 작업 이후 다시 (2) 에서 획득해야 합니다.  
	- 만약 전체 데이터 구조를 하나의 mutex 만을 가지고 보호한다면 ( 대단위 lock ),
	이로 인해 더 많은 lock 경합이 발생할 것이고, lock 에 대한 유지시간이 길어질 것입니다.
  연산 수행과정이 진행될 수록, 같은 mutex 에 대한 lock 을 필요로 할 것이고,
	이로 인해 lock 은 더 더욱 오랜 시간 유지될 것입니다.   
	- 따라서 이런 두배의 whammy 비용은 가능한한 세밀한 locking 으로 바꿈으로서 두배의 인센티브로 작용 할 수 있습니다.


- 예로 보였듯이, 적절한 *granularity locking* 이란 데이터 양에 대한 lock 뿐 아니라;
lock 의 유지 시간과, lock 유지 동안에 어떤 작업을 할 것인가를 포함합니다.
일반적으로, lock 은 가능한 필요한 작업을 하는데 걸리는 최소 시간만을 유지해야 합니다.
이것을 또한 lock 이후 또 다른 lock ( 심지어 이것이 데드락에 빠지지 않는 다는 사실을 알고 있어도 ) 을 하거나,
I/O 가 끝나는 것을 기다리는 것과 같은 시간을 많이 소비하는 작업은 절대적으로 필요하지 않는한 해선 안된다는 뜻입니다.


- Listing 3.6 과 3.9 의 교환 연산은 두개의 mutex 의 locking 을 필요로 합니다.
이는 명백하게 각각의 객체에 대해 동시적으로 접근을 필요로 합니다. 이게 어떤 차이를 만들까요?
	- 일반적인 int 형의 데이터 멤버를 비교하는 작업을 시도한다고 가정해 봅시다.
	int 는 복사 비용이 매우 적습니다,
	그래서 값의 비교 및 복사를 위해 객체 lock 을 유지하는 동안,
	각각 객체의 비교를 위한 데이터를 복사할 수 있습니다.  
	- 이것은 각각의 mutex 를 최소한의 양만큼 lock 을 유지했다는 것을 뜻하고,
	또한 locking 중 다른 lock 을 하지 않았다는 것을 뜻합니다.


- 다음의 listing 에서 보여주는 것에 따르면 클래스 Y 는 이런 케이스를 보여주며, 평등 비교 연산을 하는 예를 보여줍니다.

####Listing 3.10 Locking one mutex at a time in a comparison operator
```c++
class Y
{
	private:
		int some_detail;
		mutable std::mutex m;
		int get_detail() const
		{
			std::lock_guard<std::mutex> lock_a(m); /* (1) */
			return some_detail;
		}
	public:
		Y(int sd):some_detail(sd){}
			friend bool operator == (Y const& lhs, Y const& rhs)
			{
				if(&lhs==&rhs)
					return true;
				int const lhs_value = lhs.get_detail(); /* (2) */
				int const rhs_value = rhs.get_detail(); /* (3) */
				return lhs_value == rhs_value; /* (4) */
			}
};

```

- 이 예제에서, 비교 연산자는 멤버 함수 get_detail() (2), (3) 에서 비교될 값을 첫번째로 검색합니다.
이 함수는 (1) lock 으로 보호되는 동안에 값을 검색합니다.
비교 연산자는 검색한 값들을 비교합니다. (4)
- 하지만 이런 semantics of the operation 의 미묘한 차이는 각자의 lock 을 같이 유지했을 때와 비교하여,
locking 유지 시간이 줄어드는 만큼 한 순간에 오직 하나의 lock 이 유지된다는 점을 주의해야 합니다. ( 이것은 데드락의 가능성을 줄어듭니다 )

- listing 3.10 에서, 만약 연산자가 참을 반환하면, 이 시점에 포인트 값 lhs.some_detail 은 또다른 포인트 값 rhs.some_detail 이 같다는 것을 의미합니다.
	- 이 값은 두 읽기과정 사이에서 어떤 식으로든 변경되었을 수 있습니다;
	- 예를 들어, 이 값은 (2)와 (3) 사이에 무의미한 비교 렌더링 사이에서 교환되었을 수 있습니다.  
	인스턴트의 값들이 실제로 같은 순간이 한번도 없었음에도 불구하고 동등 연산자는 참을 리턴할 것입니다.


It’s therefore important to be careful when making such changes that the semantics of the operation are not changed in a problematic fashion:   
- 따라서 이런 *semantics of the operation* 의 변경이 있는 상황에서, *problematic fashion* 가 변하지 않는 다는 점을 주의해야 합니다. ??


- 만약에 전체 실행 과정중에 필요한 순간에 lock 을 가지고 있지 않는다면, 교착 상태에 빠질 것입니다.
때때로, 데이터 구조로의 접근이 모두 같은 수준의 레벨이 아닌 경우,  *granularity* 의 적절한 레벨이 없을 수 있습니다.
이런 상황에서는, 일반적인 std::mutex 대신에 다른 방식을 쓰는 것이 적절할 수 있습니다.

## 3.3 Alternative facilities for protecting shared data

- mutex 들이 가장 일반적으로 쓰이는 방법이지만, 공유 데이터를 보호할 수 있는 유일한 방법이 아닙니다.
이런 특정한 상황에서 좀더 적절한 보호를 제공해줄 대안들이 존재합니다.  
	- 극단적인 케이스 중 하나는 ( 하지만 일반적으로 일어나는  ) 공유 데이터가 초기화 과정에서 동시 접근에 대한 보호가 필요하지만, 이후 명시적인 동기화는 필요하지 않는 상황입니다.
  - 이것은 데이터가 읽기 전용으로 만들어 졌기 때문에,  그리고 동기화 이슈에 관한 가능성이 전혀 없고, 또는 보호가 필요한 부분은 데이터의 대한 작업의 일부로 암시적으로 행해지기 때문입니다.
- 어느 경우에나, 초기화 보호를 위해서 데이터를 초기화 한 후에 mutex 를 locking 하는 것은 불필요 하며, 성능에 쓸모없는 영향을 줄 것 입니다.
- 이것이 C++ 표준이 초기화 중 공유 데이터를 보호하기 위한 메커니즘을 제공하는 이유 입니다.


### 3.3.1 Protecting shared data during initialization

- 정말 필요하지만 생성 비용이 매우 비싼 공유 자원이 있다고 가정해 봅니다.
아마 이 자원은 데이터 베이스 연결을 열거나, 많은 메모리를 할당할 것 입니다.
이러한 *Lazy* 초기화는 싱글 스레드 코드에서 일반적으로 쓰입니다.
이는 각 연산에서 필요로 하는 자원이 있을때 이 자원이 초기화 됬는지 검사를 하고, 이것이 사용되기 전에 초기화 합니다. :


```c++
std::shared_ptr<some_resource> resource_ptr;
void foo()
{
	if(!resource_ptr)
	{
		resource_ptr.reset(new some_resource); /* (1) */
	}
	resource_ptr->do_something();
}
```

- 만약 공유 자원이 동시 접근에서 안전하다면, 이 코드를 멀티스레드 코드로 변경할 때 보호가 필요한 부분은 오직 초기화 부분 (1) 입니다.
하지만 다음에 나오는 listing 과 같은 *naive* 한 변경은 불필요하게 자원을 소비하는 스레드 직렬화를 야기 시킬 수 있습니다.
이것이 스레드가 mutex 를 기다리는 이유입니다. 이는 리소스가 이미 초기화 됬는지 여부를 체크합니다.


####Listing 3.11 Thread-safe lazy initialization using a mutex
```c++
std::shared_ptr<some_resource> resource_ptr;
std::mutex resource_mutex;
void foo()
{
	std::unique_lock<std::mutex> lk(resource_mutex);
	if(!resource_ptr)
	{
		resource_ptr.reset(new some_resource);
	}
	lk.unlock();
	resource_ptr->do_something();
}
```

- 이 코드는 불필요한 직렬화 문제를 보여주기 충분한 일반적인 코드 입니다. 이는 많은 사람들이 더 좋은 방법을 찾기 위해 노력하고 있는 악명 높은 문제입니다.
	- 포인터는 (1) 에서 우선 lock 획득 없이 읽고 ( 아래 코드에 있듯이 ),
	그리고 포인터가 NULL 일 경우만 lock 을 획득합니다.
	- 포인터는 (2) lock 을 획득하고 다시 검사 합니다.( 두번 검사 하는 부분입니다. )
	이 케이스에서 다른 스레드는 첫번째 포인터 검사와 이 스레드가 lock 을 획득하는 사이에 초기화를 마칩니다.


```c++
void undefined_behaviour_with_double_checked_locking()
{
	if(!resource_ptr) /* (1) */
	{
		std::lock_guard<std::mutex> lk(resource_mutex);
		if(!resource_ptr) /* (2) */
		{
			resource_ptr.reset(new some_resource); /* (3) */
		}
	}
	resource_ptr->do_something(); /* (4) */
}
```


- 불행히도, 이런 패턴은 다음과 같은 이유로 악명이 높습니다:  
	-  이것은 잠재적인 *nasty* 교착 상태입니다, 왜냐하면 lock 의 범위 밖에서 읽는 (1) 은 다른 스레드가 lock 범위 안에서 하는 작업 (3) 쓰기 로 인해 동기화에 실패합니다.
	- 그 결과로 이것은 자신의 포인터 뿐만 아니라 객체의 포인터 까지 포함시키는 교착 상태를 만들고;
	심지어 스레드가 다른 스레드로 인해 쓰인 포인터를 확인 한다 해도,
	그것은 ```some_resource``` 의 새로 생긴 인스턴스가 아닐 것이고,
	그 결과로 (4) ```do_somting()``` 호출 작업은 부정확한 값으로 연산을 하게 될 것입니다.
- 이는 C++ 에서 데이터 레이스로 정의되는 교착 상태의 한 종류 중 하나입니다.
그리고 이는 *undefined behavior* 로 지정되었습니다. 그러므로 이런 문제는 반드시 피해야 합니다.


- 챕터 5에서는 메모리 모델에 대한 상세와, 무엇이 데이터 레이스를 구성하는 가에 대해 논의합니다.
- C++ 표준 위원회 또한 이를 중요한 하나의 시나리오로 보기 때문에,
C++ 표준 라이브러리는 ```std::once_flag``` 와 ```std::call_once``` 를 제공하여 이런 상황을 조절할 수 있도록 제공합니다.
- mutex 를 lokcing 하고 명시적으로 포인터를 검사하는 것 보다,
모든 스레드가 ```std::call_once``` 를 사용하는 것만으로도,
포인터가 ```std::call_once``` 이 리턴하는 쓰레드에 의해 안전하게 초기화 된다는 것을 확신 할 수 있습니다.
- ```std::call_once``` 의 사용은 일반적으로 명시적인 mutex 의 사용 하고,
특히 이미 초기화가 끝난 상황에서, 기능적으로 필요한 곳에 적절히 사용 되었을 때 오버헤드가 적습니다.


- 다음의 예는 listing 3.11 과 같은 작업을 하지만, ```std::call_cone``` 를 이용하여 다시 작성하였습니다.
- 이 케이스에서, 초기화는 함수의 호출로 인해 끝났지만, 함수 호출 연산자의 클래스 인스턴스로도 쉽게 수행 될 수 있습니다.
- 인수로 함수나 조건을 받는 표준 라이브러리의 대부분 함수들과 마찬가지로,
```std::call_once``` 는 어떠한 함수나 호출 가능한 객체 와 함께 수행합니다.


```c++
std::shared_ptr<some_resource> resource_ptr;
std::once_flag resource_flag; /* (1) */
void init_resource()
{
	resource_ptr.reset(new some_resource);
}
void foo()
{
	std::call_once(resource_flag,init_resource);
	resource_ptr->do_something();
}
```

이 예제에서, 초기화 되는 ```std::once_flag``` (1) 과 데이터는 네임스페이스 범위 영역 객체입니다,
하지만 ```std::call_once``` 는 다음의 listing 에서와 같이 *Lazy* 초기화 클래스 멤버로서 쉽게 쓸 수 있습니다.


####Listing 3.12 Thread-safe lazy initialization of a class member using std::call_once
```c++
class X
{
	private:
		connection_info connection_details;
		connection_handle connection;
		std::once_flag connection_init_flag;
		void open_connection()
		{
			connection=connection_manager.open(connection_details);
		}
	public:
		X(connection_info const& connection_details_):
			connection_details(connection_details_)
	{}
		void send_data(data_packet const& data) /* (1) */
		{
			std::call_once(connection_init_flag,&X::open_connection,this); /* (2) */
			connection.send_data(data);
		}
		data_packet receive_data() /* (3) */
		{
			std::call_once(connection_init_flag,&X::open_connection,this); /* (4) */
			return connection.receive_data();
		}
};
```

- 이 예제에서, 초기화는 (1) ```send_data()``` 의 첫 호출 또는 (3) ```receive_data()``` 의 첫 호출 때 이루어 집니다.
- 데이터 초기화를 위한 멤버 함수 open_connection() 는 의 사용은, ```std::call_once``` 의 포인터 인자로 전달될 것을 요구됩니다.
- 표준 라이브러리에서 ```std::thread``` 나 ```std::bind()``` 의 생성자와 같은 호출가능한 객체를 허용 하듯이,
(2) ```std::call_once()``` 에 추가적인 인자로 전달하여 사용 할 수 있습니다.
- 이것은 말할필요도 없이 ```std::mutex``` 처럼 ```std::once_flage``` 인스턴스는 복사할수도 이동할수도 없습니다.
그렇기 때문에 이처럼 클래스 멤버로 사용하려 한다면,
명시적으로 이러한 특별한 멤버 함수가 필요하다는 것을 정의해야 합니다.


- 초기화 상황에서의 잠재적 교착 상태에 대한 시나리오 중 하나는 *static* 으로 지역 변수를 선언하는 것 입니다.
  - 이러한 변수의 초기화는 처음 변수의 선언때만 정의되도록 제어됩니다,
  이는 멀티 스레드 함수 호출에서 *define first* 하는 잠재적 교착 상태가 있다는 것을 뜻 합니다.
- C++11 이전의 많은 컴파일러에서는, 이런 교착 상태가 실질적 문제가 됩니다. ( *static* 은 thread safe 하지 않습니다. )
멀티 스레드가 자신들이 처음으로 변수를 초기화 하고 있다고 믿고 있거나,
다른 스레드가 초기화가 미쳐 끝마치기 이전에 이 것을 사용하려고 하기 때문입니다.
- C++11 에서 이런 문제가 해결 되었습니다 :
  - C++11 의 *static* 은 초기화가 정확히 한 스레드에서 일어나도록 정의하고,
  다른 어떤 스레드도 초기화가 마치기 전까지 진행할 수 없습니다.
  따라서 어떤 스레드가 초기화를 진행 할지에 대한 교착문제는 어떤 문제도 되지 않습니다.
- 이것은 단일 전역 인스턴스를 필요로 하는 케이스에서 ```std::call_once``` 를 사용하는 방법의 대안으로 쓰입니다.


```c++
class my_class;
my_class& get_my_class_instance()
{
	static my_class instance; (1)
	return instance;
}
```

- 멀티 스레드는 초기화 과정에서의 교착 상태에 대한 어떠한 걱정없이 (1) ```get_my_class_instance()``` 를 안전하게 호출할 수 있습니다.
- 초기화 과정에서만의 데이터 보호는 좀더 일반적인 시나리오에서의 특별한 경우입니다.
  - 가끔씩 갱신되는 데이터 구조가 있습니다. 이 구조는 대부분의 경우 읽기 전용이고, 멀티 스레드에 의해 동시적으로 쉽게 읽혀 집니다,
  하지만 때때로 데이터 구조의 갱신이 필요할 때가 있습니다.
  - 이런 데이터 구조는 보호 매커니즘을 필요로 합니다.


### 3.3.2 Protecting rarely updated data structures

- 도메인 네임을 올바른 IP 주소로 풀어주는 DNS 엔트리의 캐시를 저장하는 테이블을 생각해 봅시다.
  - 일반적으로 주어진 DNS 엔트리는 대부분의 경우 오랜시간 동안 변함 없이 몇년동안 남겨져 있을 것 입니다.
  유저가 다른 웹 사이트에 방문함에 따라 새로운 엔트리는 시간이 갈수록 테이블에 쌓여가고,
  그러므로 데이터는 큰 변함없이 남아있을 것 입니다.  
    - 이것이 바로 주기적인 캐시 엔트리의 유효성 검사의 중요성 입니다,
    하지만 이것은 여전히 실질적 변화가 있을 때만 갱신을 필요로 합니다.
	  - 정보의 갱신이 매우 적음에도 불구하고, 이런 일은 여전히 발생하며,
    만약 이 캐시에 멀티스레드가 접근한다면,
    어떤 스레드도 broken 데이터 구조를 읽지 않음을 보장하도록 데이터 갱신동안의 적절한 보호를 필요로 할 것입니다.
	  - 읽기와 갱신의 동시성을 고려하여 이에 맞는 특별히 디자인 ( 6 장과 7 장에 나온 ) 되거나 이런 특정한 목적에 맞는 데이터 구조가 없이는,
    이러한 종류의 업데이트는 스레드가 작업이 완료될 때 까지 데이터 구조에 상호 배제적으로 접근하는 방식이 필요로 합니다.
    변경이 끝나면, 데이터 구조는 다시금 멀티 스레드의 동시적인 접근에서 안전해 집니다.


- std::mutex 를 데이터 구조 보호에 사용하는 것은 지나치게 비관적인 방법입니다,
데이터 구조가 수정 없이 읽기만 한다면 이는 데이터의 동시적 읽기의 가능성을 제거하게 됩니다.
따라서 우리는 다른 종류의 mutex 가 필요합니다.
	- 이러한 종류의 mutex 는 전형적으로 *reader-writer mutex* 라고 부릅니다.
  이는 두가지 종류의 사용법이 있는데 배타적으로 단 하나의 *wirter* 또는 공유 스레드만 접근 가능한 방법과,
  동시적으로 읽기 가능한 *reader* 멀티 스레드들 입니다.
	- 새로운 C++ 표준 라이브러리는 표준 위원회에 제시되었음에도 불구하고, 이런 특별한 mutex 를 제공하지 않습니다.
	- 따라서, 이번 섹션의 예제에서 Boost 라이브러리를 사용합니다. 이 Boost 라이브러리는 이런 제안에 기반됩니다.


- 챕터 8 에서 다루게 되듯이, 이런 mutex 의 사용은 만변 통치약이 아닙니다,
그리고 성능은 프로세서의 갯수와 읽기와 갱신을 가진 스레드의 업무량에 좌우 됩니다.
- 타겟 시스템에서 추가적인 복잡성이 실제로 이득이 있는지 확신하기 위해선,
코드의 성능을 프로파일링 하는 것은 매우 중요합니다.
- 동기화를 위하여 ```std::mutex``` 인스턴스를 사용하기 보단, ```boost::shard_mutex``` 인스턴스를 사용해보세요.
- 갱신 작업은, ```std::lock_guard<boost::shared_mutex>``` 와 ```std::unique_lock<boost::shared_mutex>``` 는 ```std::mutex``` 를 대신하여 locking 할 수 있습니다.
	- 이는 ```std::mutex``` 와 같인 배타적인 접근을 보장합니다.
	- 이 스레드들은 ```boost::shared_lock<bost::shared_mutex>``` 를 사용하여 공유 접근을 얻음으로서 갱신이 필요 없습니다.
	- 이는 ```std::unique_lock``` 처럼 쓰이는데, 차이점은 ```boost::shared_mutex``` 에서는 멀티 스레드가 동시에 공유 lock 을 가질 수 있습니다.
	- 공유 lock 의 유일한 제약조건은, 스레드가 배타적인 lock 을 획득하려고 하면 다른 모든 스레드가 lock 을 해제 하기 전까지 블록 당할 것 입니다.
	그리고 스레드가 배타 lock 을 가지고
	있다면, 다른 어느 스레드도 이 배타 lock 이 해제되기 전까진 공유 lock 이나 배타 lock 을 가질 수 없습니다.

- 다음의 listing 은 위에서 묘사했던 간단한 DNS 캐시를 보여주며, ```std::map``` 을 이용하여 캐시 데이터를 보유하고, ```boost::shared_mutex``` 를 이용하여 보호합니다.


####Listing 3.13 Protecting a data structure with a boost::shared_mutex
```c++
#include <map>
#include <string>
#include <mutex>
#include <boost/thread/shared_mutex.hpp>

class dns_entry;
class dns_cache
{
	std::map<std::string,dns_entry> entries;
	mutable boost::shared_mutex entry_mutex;
public:
	dns_entry find_entry(std::string const& domain) const
	{
		boost::shared_lock<boost::shared_mutex> lk(entry_mutex); /* (1) */
		std::map<std::string,dns_entry>::const_iterator const it=
			entries.find(domain);
		return (it==entries.end())?dns_entry():it->second;
	}
	void update_or_add_entry(std::string const& domain,
			dns_entry const& dns_details)
	{
		std::lock_guard<boost::shared_mutex> lk(entry_mutex); /* (1) */
		entries[domain]=dns_details;
	}
};
```

- listing 3.13 에서 ```find_entry()``` 는 ```boost::shraed_lock<>``` 인스턴스를 이용하여, 공유, (1) *read-only* 접근을 보호 합니다.
그렇기 때문에 다른 멀티 스레드들도 문제 없이 ```find_entry()``` 를 동시적으로 호출 가능합니다.  
- 반면에, ```update_or_add_entry()``` 는 ```std::lock_guard<>``` 인스턴스를 사용하는데,
	이는 테이블이 (2) 갱신되는 동안 배타적인 접근을 제공합니다;
	- 이는 다른 스레드가 ```update_or_add_entry()``` 를 호출하여 데이터를 갱신하는 것을 방지할뿐만 아니라,
	```find_entry()``` 를 호출하는 다른 스레드 모두를 블록합니다.


### 3.3.3 Recursive locking

- ```std::mutex``` 에서는,
mutex 가 lock 을 이미 가진 상태에서 다시금 lock 을 시도하면 에러가 발생합니다,
그리고 *undefined behavior* 를 발생시킬 것 입니다.
하지만, 스레드가 같은 mutex 를 첫 번째 획득한 lock 의 해제 없이 재획득 해야만 하는 상황이 있습니다.
이런 경우를 위해, C++ 표준 라이브러리는 ```std::recursive_mutex``` 를 제공합니다.   
	- ```std::mutex``` 와 동일한 기능을 하지만,
	같은 스레드의 단일 인스턴스에 대해서 반복적으로  lock 을 획득 할 수 있습니다.  
	- 다른 스레드 mutex 의 lock 을 획득하기 위해선 당신이 획득한 모든 lock 을 해제해야 합니다.  
		- ```lock()``` 을 세번 호출했다면, ```unlock()``` 역시 3번 호출해야 합니다.
- 올바른 ```std::lock_gurad<std::recursivd_mutex>``` 와 ```std::unique_lock<std::recursive_mutex>``` 의 사용은 이런 문제를 해결합니다.
만약 *recursive* mutex 를 사용하기 원한다면, 설계를 바꿔야 할 것입니다.
- 일반적으로  *recursive* mutex 는 클래스에서 멤버 데이터에 대한 멀티 스레드의 동시적 접근으로부터 보호하기 위해 사용합니다.


- 각각의 퍼블릭 멤버 함수는 mutex 를 lock 그리고 unlock 합니다.
하지만 때때로, 하나의 퍼블릭 멤버 함수가 오퍼레이션의 한 일부분으로 다른 함수를 호출해야 할 때가 있습니다.
이런 상황에서, 두번째 멤버 함수는 mutex 에 lock 을 시도하게 되고, 이로인해 *undefine behavior* 가 발생합니다.
- *quick-and-dirty* 한 해결 방법은 mutex 를 *recursive* mutex 로 바꾸는 것 입니다.
이것은 mutex lock 이 두번째 멤버 함수에 성공하고, 이 함수가 진행을 계속 하는 것을 허용합니다.
하지만, 이 방법은 추천할 만한 방법은 아닌데, 안좋은 설계와 *sloppy* 한 생각으로 이끌 수 있기 때문입니다.


In particular, the class invariants are typically broken while the lock is held, which means that the second member function needs to work even when called with the invariants broken.
클래스의 *invariants* 는 보통 lock 이 유지되는 동안에 *broken* 되는데, 이것은 두번째 멤버 함수는 *invariants* 가 *broken* 된 상황에서 work 하기를 원한 다는 것을 뜻합니다. ????


It’s usually better to extract a new private member function that’s called from both member functions, which does not lock the mutex (it expects it to already be locked).
이것은 대체로 새로운 private 멤버 함수를 extract 하는 것이 mutex 가 lock 되지 않은 ( 이미 lock 되어 있을 거라고 예상하는 ) 함수를 호출하는 것보다 낫습니다.  ??


You can then think carefully about the circumstances under which that new function can be called and the state of the data under those circumstances.
이러한 상황에서의 새로운 함수나 데이터의 호출은 다시금 생각해 봐야 합니다. ????



## Summary

- 이번 챕터는 스레드 간 데이터 공유할 때 발생할 수 있는 교착 상태 문제와, std::mutex 사용법 그리고 이런 문제를 피할 인터페이스 구축을 하는 방법에 대하여 논의하였습니다.
이를 통해 mutex 들이 만병 통치약이 아닌 것을 있었고, 이 mutex 로 인해 생기는 데드락 문제를 보았습니다.
그리고 C++ 표준 라이브러리가 제공하는 ```std::lock()``` 란 형태의 도구를 제공하여 이를 피하는 방법 역시 볼 수 있었습니다.

- 데드락을 피하는 몇가지 테크닉인 소유권 이전과 적절한 locking 범위를 선택하는 이슈를 간단히 살펴보았습니다.
마지막으로, 대안 방법인 특정 상황에서 제공하는 데이터 보호 기능인 ```std::call_once()``` 와 ```boost::shared_mutex``` 와 같은 기능을 다뤘습니다.  
하지만 한가지 아직 다루지 않은 이슈가 있는데, 이것은 다른 스레드 부터의 입력을 기다리는 경우 입니다.

- 스레드 안정적인 스택을 가정해봅시다. 만약 스택이 비어있다면 예외상황을 일으킬 것이고,
한 스레드는 다른 스레드가( 스레드 안정적인 스택의 다른 주요 사용자 ) 스택에 데이터를 넣기를 기다리고 있습니다,
이 경우 스레드는 데이터를 반복적으로 pop 하는 것을 시도하고, 만약 에러가 발생하면 재시도를 합니다.  
이런 소비적인 작업은 어떠한 실제적인 작업은 진행하지 않고, 검사를 수행하는데에만 시간을 허비합니다.
실제로, 실행중인 시스템에서 일정하고 주기적인 검사는 다른 스레드의 작업을 방해할 수 있습니다.
이 때 필요한 것은 다른 스레드가 작업을 완료시킬때 CPU 의 자원 소비없이 기다리는 방법 입니다.


- 챕터 4는 공유 데이터를 보호하기 위한 기능의 생성에 대해 논의하고, C++ 에서 스레드 간의 동기화 작업에 대한 다양한 메커니즘을 소개합니다.
그리고 챕터 6 에서는 이러한 재사용 가능한 데이터 구조를 어떻게 만드는지 보여줍니다.
