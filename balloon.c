#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>

#define MAX_BALLOONS 40
#define MAX_T 100
#define MYNULL -1

/******************************************************************************
 * boardの情報
 *****************************************************************************/
/* 風船 struct / balloon struct */
typedef struct balloon
{
  int time;
  int pos;
} balloon_t;

/* 風船が落ちてくる時のロボットの状態 */
typedef struct robot
{
  int traveledDistance;
  bool isOk;
} robot_t;

/*
 * 結果 struct
 * Result struct
 */
typedef struct result
{
  int isOK;
  int num;
} result_t;

/* 風船の配列 Balloon array */
balloon_t balloons[MAX_BALLOONS];

/*
 * 次のｎ風船をファイルから読み込む
 * Parse the next n balloons from the file
 */
void setupBalloons(FILE *in, int n)
{
  int i = 0;
  for (i = 0; i < n; i++)
  {
    int time, pos;
    fscanf(in, "%d %d", &pos, &time);
    balloons[i].time = time;
    balloons[i].pos = pos;
  }
}

/*ロボットの状態の配列*/
robot_t nextRobotStates[4];
robot_t currentRobotStates[4];

/*ロボットの状態の配列の初期化*/
bool initRobotStates()
{
  for (size_t i = 0; i < 4; i++)
  {
    currentRobotStates[i].isOk = false;
    currentRobotStates[i].traveledDistance = MYNULL;
  }

  for (size_t i = 0; i < 4; i++)
  {
    nextRobotStates[i].traveledDistance = MYNULL;
    nextRobotStates[i].isOk = false;
  }
  nextRobotStates[1].traveledDistance = balloons[0].pos;
  nextRobotStates[1].isOk = balloons[0].pos <= balloons[0].time ? true : false;
  return nextRobotStates[1].isOk;
}

int findMinNonNull(int nums[], int size)
{
  int min = MYNULL;
  for (size_t i = 0; i < size; i++)
  {
    if (nums[i] == MYNULL)
    {
      continue;
    }

    if (min == MYNULL)
    {
      min = nums[i];
      continue;
    }

    min = nums[i] < min ? nums[i] : min;
  }
  return min;
}

/*
 * balloons配列に記録されている問題を解ける
 * solve the problem contained in the balloons array
 *
 * parameter:
 * - n: 風船の数 number of balloons
 */
result_t solve(int n)
{
  result_t result;
  if (!initRobotStates())
  {
    result.isOK = false;
    result.num = 1;
    return result;
  }
  for (size_t i = 1; i < n; i++)
  {
    // 状態の更新
    currentRobotStates[1].isOk = nextRobotStates[1].isOk;
    currentRobotStates[2].isOk = nextRobotStates[2].isOk;
    currentRobotStates[3].isOk = nextRobotStates[3].isOk;
    currentRobotStates[1].traveledDistance = nextRobotStates[1].traveledDistance;
    currentRobotStates[2].traveledDistance = nextRobotStates[2].traveledDistance;
    currentRobotStates[3].traveledDistance = nextRobotStates[3].traveledDistance;

    int time = balloons[i].time - balloons[i - 1].time;
    int currentPostion = balloons[i - 1].pos;
    int nextPostion = balloons[i].pos;
    int distance = abs(currentPostion - nextPostion);

    // 次のステップでrobotがballoonを1個保有している場合
    int array[] = {MYNULL, MYNULL, MYNULL};
    int distanceViaHome = currentPostion + nextPostion;

    if (currentRobotStates[1].isOk && (currentPostion * 2 + nextPostion) <= time)
    {
      array[0] = currentRobotStates[1].traveledDistance + distanceViaHome;
    }

    if (currentRobotStates[2].isOk && (currentPostion * 3 + nextPostion) <= time)
    {
      array[1] = currentRobotStates[2].traveledDistance + distanceViaHome;
    }

    if (currentRobotStates[3].isOk && (currentPostion * 4 + nextPostion) <= time)
    {
      array[2] = currentRobotStates[3].traveledDistance + distanceViaHome;
    }

    int minDistance = findMinNonNull(array, 3);
    nextRobotStates[1].isOk = minDistance == MYNULL ? false : true;
    nextRobotStates[1].traveledDistance = minDistance;

    // 次のステップでrobotがballoonを2個保有している場合
    if (currentRobotStates[1].isOk && (distance * 2) <= time)
    {
      nextRobotStates[2].isOk = true;
      nextRobotStates[2].traveledDistance = currentRobotStates[1].traveledDistance + distance;
    }

    // 次のステップでrobotがballoonを3個保有している場合
    if (currentRobotStates[2].isOk && (distance * 3) <= time)
    {
      nextRobotStates[3].isOk = true;
      nextRobotStates[3].traveledDistance = currentRobotStates[2].traveledDistance + distance;
    }

    // 現時点でrobotがどれかの状態でballoonをすべて回収できているか確認する
    if (!nextRobotStates[1].isOk && !nextRobotStates[2].isOk && !nextRobotStates[3].isOk)
    {
      result.isOK = false;
      result.num = 1 + i;
      return result;
    }
  }

  int array[] = {
      nextRobotStates[1].traveledDistance,
      nextRobotStates[2].traveledDistance,
      nextRobotStates[3].traveledDistance};

  printf("1a=%d 2a=%d 3a=%d\n", array[0], array[1], array[2]);
  result.num = findMinNonNull(array, 3) + balloons[n - 1].pos; // 家に帰る距離を足す
  result.isOK = true;
  return result;
}

/*******
 * こちらで用意したmain 関数。
 * 問題準備してから、solve() をよび、正解比較もおこなう。
 */
int main(int argc, char *argv[])
{
  struct
  {
    char *in, *ans;
  } filePairs[] = {{"sample.in", "sample.ans"},
                   {"B.in", "B.ans"}};

  int i;
  for (i = 0; i < 2; i++)
  {
    char *inFile = filePairs[i].in;
    char *ansFile = filePairs[i].ans;
    FILE *in = fopen(inFile, "r");
    FILE *ansIn = fopen(ansFile, "r");
    if (in == NULL)
    {
      printf("Can't open file: %s.\n", inFile);
      return 0;
    }
    if (ansIn == NULL)
    {
      printf("Can't open file: %s.\n", ansFile);
      return 0;
    }
    int failCount = 0;
    int totalCount = 0;
    printf("Processing input: %s\n", inFile);
    while (1)
    {
      int n, ans;
      char isOK[10];
      result_t result;
      fscanf(in, "%d", &n);
      if (n == 0)
        break;
      setupBalloons(in, n);
      result = solve(n);
      fscanf(ansIn, "%s %d", isOK, &ans);
      if ((strcmp("OK", isOK) == 0) != result.isOK || ans != result.num)
      {
        failCount++;
        printf("You failed problem No. %d (result: %s %d, ans: %s, %d)\n",
               totalCount, result.isOK ? "OK" : "NG", result.num, isOK, ans);
      }
      totalCount++;
    }
    if (failCount == 0)
    {
      printf("Congratulation! You passed all problems (%d) in %s!\n", totalCount, inFile);
    }
    else
    {
      printf("Im sorry you missed %d/%d in %s!\n", failCount, totalCount, inFile);
      return 0;
    }
  }
  return 0;
}
