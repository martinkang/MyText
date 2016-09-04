#include "maze.h"

/* 미로를 복합하는데 필요한 인터페이스를 정의 
   어떤 구현도 정의하지 않는다. 이는 Builder 의 서브클래스에서
   관심 있을 때만 이들 연산을 재정의할 수 있도록 하기 위해서 */
/* 미로 자체를 만들지 않고 단지 미로를 생성하는 인터페이스를 정의 */
class MazeBuilder
{
public:
	virtual void BuildMaze() {}
	virtual void BuildRoom( int aRoomNo ) {}
	virtual void BuildDoor( int aRoomFrom, int aRoomTo ) {}

	virtual Maze * GetMaze() { return 0; }

protected:
	MazeBuilder();

};

/* 미로의 내부 표현을 은닉하였다.
   어떤 미로인지, 방과 문의 관계는 알 수 없고 
   오로지 구성 요소를 만들 뿐이다.
   이 때문에 builder 의 메서드만 재정의 하면 
   새로운 형태의 미로를 만들 수 있게 된다. */
Maze * MazeGame::CreateMaze( MazeBuilder & aBuilder )
{
	aBuilder.BuildMaze();

	aBuilder.BuildRoom( 1 );
	aBuilder.BuildRoom( 2 );
	aBuilder.BuildDoor( 1, 2 );

	return aBuilder.GetMaze();
};

Maze * MazeGame::CreateComplexMaze( MazeBuilder & aBuilder )
{
	aBuilder.BuildRoom( 1 );
	aBuilder.BuildRoom( 2 );
	aBuilder.BuildRoom( 3 );

	aBuilder.BuildDoor( 1, 2 );
	aBuilder.BuildDoor( 1, 3 );
	
	return aBuilder.GetMaze();
};


class StandardMazeBuilder : public MazeBuilder
{
public:
	StandardMazeBuilder()
	{
		_currentMaze = NULL;
	};

	virtual void BuildMaze()
	{
		_currentMaze = new Maze;
	};

	virtual void BuildRoom( int aRoomNo )
	{
		if ( _currentMaze->getRoomByNo( aRoomNo ) != NULL )
		{
			Room * sRoom = new Room( aRoomNo );
			_currentMaze->AddRoom( sRoom );

			sRoom->SetSide( North, new Wall );
			sRoom->SetSide( South, new Wall );
			sRoom->SetSide( East, new Wall );
			sRoom->SetSide( West, new Wall );
		}
		else
		{
			cout << "Already Room_No " << aRoomNo << "Exist!" << endl;
		}
	};

	virtual void BuildDoor( int aRoomFrom, int aRoomTo )
	{
		Room * sR1 = _currentMaze->getRoomByNo( aRoomFrom );
		Room * sR2 = _currentMaze->getRoomByNo( aRoomTo );

		if ( ( sR1 != NULL ) && ( sR2 != NULL) )
		{
			Door *sD = new Door( sR1, sR2 );
			
			sR1->SetSide( CommonWall( sR1, sR2 ), sD );
			sR2->SetSide( CommonWall( sR2, sR1 ), sD );
		}
		else
		{
			if ( sR1 == NULL )
			{
				cout << "Room_No " << aRoomFrom << "does not Exist!" << endl;
			}

			if ( sR2 == NULL )
			{
				cout << "Room_No " << aRoomTo << "does not Exist!" << endl;
			}
		}
	}

	virtual Maze * GetMaze()
	{
		return _currentMaze;
	}

private:
	Maze * _currentMaze;
	
	Direction CommonWall( Room * aR1, Room * aR2 )
	{
		int i = 0;

		GetDirection( aR1, aR2, North, South );
	};

	Direction GetDirection( Room * aR1, 
			Room * aR2, 
			Direction aD1, 
			Direction aD2 )
	{
		if ( aR1->GetSide[aD1]->GetType() == Wall_t )
		{
			if ( aR1->GetSide[aD2]->GetType() == Wall_t )
			{
				return aD1;
			}
		}	

		return Invalid;
	};
};

class CountingMazeBuilder : public MazeBuilder
{
public:
	CountingMazeBuilder()
	{
		_doors = 0;
		_rooms = 0;
	}

	virtual void BuildMaze();
	virtual void BuildRoom( int );
	{
		_rooms++;
	}

	virtual void BuildDoor( int, int );
	{
		_doors++;
	}

	virtual void AddWall( int, Direction );

	void GetCounts( int & aRooms, int & aDoors ) const
	{
		aRooms = _rooms;
		aDoors = _doors;
	}

private:
	int _doors;
	int _rooms;
};


int main( void )
{
	int sRooms = 0;
	int sDoors = 0;

	MazeGame sGame;
	CountingMazeBuilder sCountingBuilder;

	sGame.CreateMaze( sCountingBuilder );
	sCountingBuilder.GetCounts( sRooms, sDoors );

	cout << "The Maze Has " << sRooms << " Rooms And "
		<< sDoors <<" Doors" << endl;

	return 0;
}