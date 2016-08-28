# Builder

## 의도
* 복잡한 객체의 생성 루틴을 객체의 표현과 분리하하여
동일한 생성 절차에서 서로 다른 표현 결과를 만들 수 있게 한다.


## 동기
* RTF ( Rich Text Format ) 문서 판독기가 있다. 
이 판독기는 RTF 포맷에서 다른 텍스트 포맷으로 바꿀 수 있어야 한다. 
어떻게 설계 하는게 좋을까?
	* 조건
		- 문서 형식들 간의 변환 가능성에 재한이 없으며, 판독기의 변경 없이도 새로운 형태의 변환이 추가될 수 있어야 한다.
	* 방법
		- RTF 를 다른 문자식으로 표현으로 변형하는 일을 맡은 TextConverter 객체와, RTFReader 로 나눈다.
			- RTFReader 는 RTF 가 토큰을 판독할 때마다 TextConverter 에 요청을 한다.
			- TextConverter 객체는 데이터 변환을 수행하거나 어떤 특별한 형태로 토큰을 표현한다.
		- TextConverter 의 서브클래스는 서로 다른 변환과 포맷을 처리할 수 있도록 분기한다.
			- ASCIIConverter 클래스는 아스키 텍스트으 변환만 처리
			- TexConverter 클래스는 TEX 형식의 문서를 생성하기 위해 필요한 모든 처리를 담당할 연산을 구현
		- RTF 를 변환기와 판독기로 분리시킨다.
			- 판독기 -> Director 
			- 변확기 클래스 -> Builder


## 활용성
	- 복합 객체의 생성 알고리즘이 이를 합성하는 요소 객체들이 무엇인지 이들의 조립 방법에 독립적일 때
	- 합성 객체들의 표현이 서로 다르더라도 생성 절차에서 이를 지원해야 할 때


#### Builder 구조
![img1](https://upload.wikimedia.org/wikipedia/commons/thumb/f/f3/Builder_UML_class_diagram.svg/700px-Builder_UML_class_diagram.svg.png)

## 참여자
* Builder ( TextConveter )
	- Product 객체의 일부 요소들을 생성하기 위한 추상 인터페이스를 정의
* ConcreteBuilder ( ASCIIConverter, TexConverter... )
	- 빌더 클래스에 정의된 인터페이스를 구현하며, 제품의 부품들을 모아 빌더를 복합한다.
	- 생성한 요소의 표현을 정의하고 관리한다.
* Director ( RTFReader )
	- 빌더 인터페이스를 사용하는 객체를 합성한다.
* Product ( ASCIIText, TeXText )
	- 생성할 복합 객체를 표현.


## 결과
* 장점
	- 제품에 대한 내부 표현을 다양하게 변화할 수 있다.
	- 생성과 표현에 필요한 코드를 분리한다.
	- 복합 객체를 생성하는 절차를 좀더 세밀하게 나눌 수 있다.


## 구현
- 제품에 대한 추상 클래스는 필요 없는가?
	- 일반적으로 제품은 상세화된 빌더 클래스의 서브 클래스로 생성되는데, 제품마다 그 제품을 표현하는 방법이 다르고
	이것에서 어떠한 공통점도 찾을 수 없기 때문에, 추상 클래스의 이점이 없다.
- Builder 에 있는 메서드에 대해서는 구현을 제공하지 않는게 일반적
	- C++ 로 구현할 때는, 빌더에 정의된 메서드를 의도적으로 가상 함수로 정의하지 않는다.
		- 구현부를 비워둔다.
	- 이는 서브클래스에서 모든 가상 함수가 아니고 필요한 메서드만 재정의 하기 위해서.


## 기타
	- Factory Method 와 Builder 의 차이는 ?
		- 생성 시점의 차이?
			- Factory 는 그때 그때 인풋에 따라 동적으로 생성을 해주고
			Builder 는 미리 만들어진 객체의 틀을 불러온다.