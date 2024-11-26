#include "bluetooth.h"

int main(void)
{
    int result = bluetoothGate(); // bluetoothInput 함수 호출
    printf("\nResult: %d\n", result); // 결과 출력
    return 0;
}
