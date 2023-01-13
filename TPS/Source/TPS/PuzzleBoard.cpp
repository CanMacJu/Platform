// Fill out your copyright notice in the Description page of Project Settings.


#include "PuzzleBoard.h"
#include "PuzzlePiece.h"
#include "PuzzlePawn.h"
#include <queue>
#include <vector>
#include <unordered_set>
#include <functional>

// Sets default values
APuzzleBoard::APuzzleBoard()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Board = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BOARD"));
	RootComponent = Board;

}

// Called when the game starts or when spawned
void APuzzleBoard::BeginPlay()
{
	Super::BeginPlay();
	
	Init();
	Player->Init(this);

}

// Called every frame
void APuzzleBoard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsMovePiece)
	{
		MovePiece(SelectIndex, BlankIndex);
	}
}

void APuzzleBoard::MovePiece(int32 From, int32 To)
{
	int32 Row = BlankIndex / Size;
	int32 Col = BlankIndex % Size;

	int32 SelectRow = From / Size;
	int32 SelectCol = From % Size;

	int32 Direct;
	if (Row + 1 == SelectRow && Col == SelectCol)
		Direct = UP;
	if (Row == SelectRow && Col - 1 == SelectCol)
		Direct = RIGHT;
	if (Row - 1 == SelectRow && Col == SelectCol)
		Direct = DOWN;
	if (Row == SelectRow && Col + 1 == SelectCol)
		Direct = LEFT;

	FVector FromLocation = IndexLocation[From];
	FVector ToLocation = IndexLocation[To];

	FVector CurrLocation = Pieces[From]->GetActorLocation();

	FVector NextLocation = CurrLocation + GetWorld()->GetDeltaSeconds() * Dir[Direct] * SwapSpeed;

	Pieces[From]->SetActorLocation(NextLocation);

	CurrLocation = Pieces[From]->GetActorLocation();

	if (GetPieceSize() <= FVector::Distance(FromLocation, CurrLocation))
	{
		Pieces[From]->SetActorLocation(ToLocation);
		Pieces[To]->SetActorLocation(FromLocation);
		IsMovePiece = false;
		RefreshBoard();
	}
}

struct Node
{
	bool operator<(const Node& other) const { return f < other.f; };
	bool operator>(const Node& other) const { return f > other.f; };

	std::vector<int32> IndexDatas;
	int32 BlankIdx;

	int32 f;
	int32 g;

	int32 Key;		// 부모 추적을 위한 자신의 parents 배열에서의 Index, 값은 PieceIndex ( parents.size() )
	int32 parentKey;	// 부모의 parents 배열에서의 Index
};

struct VectorHasher
{
	std::size_t operator()(const std::vector<int>& v) const
	{
		std::size_t seed = 0;
		for (int i : v) {
			seed ^= std::hash<int>()(i) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		}
		return seed;
	}
};

void APuzzleBoard::AStar()
{
	std::priority_queue<Node, std::vector<Node>, std::greater<Node>> pq;
	std::vector<int32> Parents;
	std::vector<int32> Blanks;
	std::unordered_set<std::size_t> Explored;
	int32 Index = 0;

	{
		std::vector<int32> IndexDatas = GetIndexData();
		int32 BlankIdx = BlankIndex;

		int32 g = 0;
		int32 h = GetHeuristic(IndexDatas);

		int32 Key = 0;
		int32 parentKey = 0;
		Parents.push_back(parentKey);
		Blanks.push_back(BlankIdx);

		pq.push(Node{ IndexDatas, BlankIdx, g + h, g, Key, parentKey });
	}

	while (pq.empty() == false)
	{
		Node node = pq.top();
		pq.pop();

		if (GetIsCorrect(node.IndexDatas))
		{
			Index = node.Key;
			break;
		}

		for (int32 i = 0; i < DIR_COUNT; ++i)
		{
			int32 NextBlankIdx = GetNextBlankIdx(node.BlankIdx, i);
			if (NextBlankIdx == node.BlankIdx)
				continue;

			if (NextBlankIdx == Blanks[node.parentKey])
				continue;

			{
				std::vector<int32> IndexDatas = node.IndexDatas;
				::Swap(IndexDatas[NextBlankIdx], IndexDatas[node.BlankIdx]);
				std::size_t hash_value = VectorHasher()(IndexDatas);
				if (Explored.count(hash_value) == 1)
					continue;
				else
					Explored.insert(hash_value);

				int32 BlankIdx = NextBlankIdx;

				int32 g = node.g + 10;
				int32 h = GetHeuristic(IndexDatas);

				int32 Key = Parents.size();
				int32 parentKey = node.Key;
				Parents.push_back(parentKey);
				Blanks.push_back(BlankIdx);

				pq.push(Node{ IndexDatas, BlankIdx, g + h, g, Key, parentKey });
			}
		}
	}

	Path.clear();

	while (true)
	{
		if (Index == Parents[Index])
			break;

		Path.push_back(Blanks[Index]);

		Index = Parents[Index];
	}

	UE_LOG(LogTemp, Warning, TEXT("Size:	%i"), Parents.size());
	UE_LOG(LogTemp, Warning, TEXT("Count:	%i"), Path.size());

	SwapSpeed = Path.size() * 450.f / Size / TimeOut;

	if (IsAI && Path.empty() == false)
	{
		int32 idx = Path.back();
		Path.pop_back();

		SelectPiece(idx);
	}

	//std::reverse(Path.begin(), Path.end());
}

int32 APuzzleBoard::GetHeuristic(std::vector<int32> IndexDatas)
{
	int32 total = 0;
	// 유클리드 거리
	for (int32 i = 0; i < Size * Size; ++i)
	{
		int32 CorrectRow = i / Size;
		int32 CorrectCol = i % Size;

		int32 CurrentRow = IndexDatas[i] / Size;
		int32 CurrentCol = IndexDatas[i] % Size;

		int32 Y = CorrectRow - CurrentRow;
		int32 X = CorrectCol - CurrentCol;

		total += static_cast<int32>(::sqrt(::pow(X, 2) + ::pow(Y, 2)) * 10.0);
	}
	// 맨해튼 거리
	/*for (int32 i = 0; i < Size * Size; ++i)
	{
		int32 CorrectRow = i / Size;
		int32 CorrectCol = i % Size;

		int32 CurrentRow = IndexDatas[i] / Size;
		int32 CurrentCol = IndexDatas[i] % Size;

		total += ::abs(CorrectRow - CurrentRow) + ::abs(CorrectCol - CurrentCol);
	}*/

	return total * 10;
}

bool APuzzleBoard::GetIsCorrect(std::vector<int32> IndexDatas)
{
	for (int32 i = 0; i < Size * Size; ++i)
	{
		if (IndexDatas[i] != i)
			return false;
	}

	return true;
}

int32 APuzzleBoard::GetNextBlankIdx(int32 Index, int32 DIR)
{
	int32 Row = Index / Size;
	int32 Col = Index % Size;

	switch (DIR)
	{
	case 0:
		if (Row >= 1)
			return Index - Size;
		break;
	case 1:
		if (Col + 1 < Size)
			return Index + 1;
		break;
	case 2:
		if (Row + 1 < Size)
			return Index + Size;
		break;
	case 3:
		if (Col >= 1)
			return Index - 1;
		break;
	default:
		break;
	}

	return Index;
}

std::vector<int32> APuzzleBoard::GetIndexData()
{
	std::vector<int32> temp;

	for (int32 i = 0; i < Size * Size; ++i)
	{
		temp.push_back(Pieces[i]->CorrectIndex);
	}

	return temp;
}

void APuzzleBoard::SelectPiece(int32 _SelectIndex)
{
	SelectIndex = _SelectIndex;
	IsMovePiece = true;
}

void APuzzleBoard::Init()
{
	Player = Cast<APuzzlePawn>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (Player == nullptr)
		return;

	for (int32 i = 0; i < Size * Size; ++i)
	{
		int32 row = i / Size;
		int32 col = i % Size;
		float X = -225.f + (row * 450.f / Size) + (450.f / Size / 2.f);
		float Y = -25.f + (-1 * col * 450.f / Size) - (450.f / Size / 2.f);

		FVector GlobalIndexLocation = GetTransform().TransformPositionNoScale(FVector(X, Y, 50.f));
		IndexLocation.Push(GlobalIndexLocation);
	}

	IsMovePiece = false;
	BlankIndex = Size * Size - 1;
	SelectIndex = BlankIndex;
}

void APuzzleBoard::ShuffleBoard()
{
	::srand((unsigned int)time(nullptr));

	for (int32 i = 0; i < 300; ++i)
	{
		int32 RandDir = ::rand() % DIR_COUNT;
		SetRandIndex(RandDir);
		while (CanMove(SelectIndex) == false)
		{
			RandDir = ::rand() % DIR_COUNT;
			SetRandIndex(RandDir);
		}

		UpdatePieceData();
	}

	SetPieceLocation();
}

void APuzzleBoard::RefreshBoard()
{
	UpdatePieceData();
	bool IsCorrect = CheckCorrect();

	if (IsCorrect == false && IsAI && Path.empty() == false)
	{
		int32 nextIndex = Path.back();
		Path.pop_back();

		SelectPiece(nextIndex);
	}
}

void APuzzleBoard::UpdatePieceData()
{
	::Swap(Pieces[SelectIndex], Pieces[BlankIndex]);
	::Swap(Pieces[SelectIndex]->CurrentIndex, Pieces[BlankIndex]->CurrentIndex);
	::Swap(SelectIndex, BlankIndex);
}

bool APuzzleBoard::CheckCorrect()
{
	for (auto Piece : Pieces)
	{
		if (Piece->CheckIndex())
			continue;
		else
			return false;
	}

	if (IsAI)
	{
		UE_LOG(LogTemp, Warning, TEXT("Complete and Shuffle"));
		ShuffleBoard();
		AStar();
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("Complete"));


	return true;
}

bool APuzzleBoard::CanMove(int32 _SelectIndex)
{
	int32 Row = BlankIndex / Size;
	int32 Col = BlankIndex % Size;

	int32 SelectRow = _SelectIndex / Size;
	int32 SelectCol = _SelectIndex % Size;

	if (Row - 1 == SelectRow && Col == SelectCol)
		return true;
	if (Row + 1 == SelectRow && Col == SelectCol)
		return true;
	if (Row == SelectRow && Col - 1 == SelectCol)
		return true;
	if (Row == SelectRow && Col + 1 == SelectCol)
		return true;

	return false;
}

void APuzzleBoard::SetSpawn(int32 _Size, float _SwapSpeed, bool _IsAI)
{
	Size = FMath::Clamp(_Size, 1, 5);
	SwapSpeed = FMath::Clamp(_SwapSpeed, 500.f, 1000.f);
	IsAI = _IsAI;
}

void APuzzleBoard::SetPieceLocation()
{
	for (int32 i = 0; i < Size * Size; ++i)
	{
		Pieces[i]->SetActorLocation(IndexLocation[i]);
	}
}

void APuzzleBoard::SetMainPiece(APuzzlePiece* _MainPiece)
{
	MainPiece = _MainPiece;

	FVector Loc = GetTransform().TransformPositionNoScale(FVector(0.f, 250.f, 50.f));
	MainPiece->SetActorLocation(Loc);
}

void APuzzleBoard::SetRandIndex(int32 randNum)
{
	switch (randNum)
	{
	case 0:
		SelectIndex = FMath::Clamp(BlankIndex + 1, 0, Size * Size - 1);
		break;
	case 1:
		SelectIndex = FMath::Clamp(BlankIndex + Size, 0, Size * Size - 1);
		break;
	case 2:
		SelectIndex = FMath::Clamp(BlankIndex - 1, 0, Size * Size - 1);
		break;
	case 3:
		SelectIndex = FMath::Clamp(BlankIndex - Size, 0, Size * Size - 1);
		break;
	default:
		SelectIndex = BlankIndex;
		break;
	}
}
