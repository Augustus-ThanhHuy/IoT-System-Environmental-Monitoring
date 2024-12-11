/* 
 * Author: Sano
 * Created on: 2024-10-11
 */
#ifndef ADC_DUST_H
#define ADC_DUST_H

#include <stdint.h> // Thêm dòng này để định nghĩa uint32_t

// Khởi tạo module ADC (bao gồm ADC và Timer nếu cần)
void ADC_Module_Init(void);

// Đọc giá trị ADC từ cảm biến bụi
uint32_t ADC_Module_ReadValue(void);

// Chuyển đổi giá trị ADC thành điện áp
float ADC_Module_GetVoltage(uint32_t adcValue);

// Tính toán mật độ bụi từ giá trị điện áp
float ADC_Module_GetDustDensity(float voltage);

// Hàm tổng hợp để đọc mật độ bụi
float ADC_Module_ReadDustDensity(void);

#endif /* ADC_DUST_H */