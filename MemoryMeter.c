/*
htop - MemoryMeter.c
(C) 2004-2011 Hisham H. Muhammad
Released under the GNU GPLv2+, see the COPYING file
in the source distribution for its full text.
*/

#include "config.h" // IWYU pragma: keep

#include "MemoryMeter.h"

#include <assert.h>
#include <math.h>
#include <stddef.h>

#include "CRT.h"
#include "Macros.h"
#include "Object.h"
#include "Platform.h"
#include "RichString.h"


static const int MemoryMeter_attributes[] = {
   MEMORY_USED,
   MEMORY_SHARED,
   MEMORY_COMPRESSED,
   MEMORY_BUFFERS,
    MEMORY_CACHE};

static void MemoryMeter_updateValues(Meter* this) {
   char* buffer = this->txtBuffer;
   size_t size = sizeof(this->txtBuffer);
   int written;

   /* shared, compressed and available memory are not supported on all platforms */
   this->values[MEMORY_METER_SHARED] = NAN;
   this->values[MEMORY_METER_COMPRESSED] = NAN;
   this->values[MEMORY_METER_AVAILABLE] = NAN;
   Platform_setMemoryValues(this);

   /* Do not print available memory in bar mode */
   static_assert(MEMORY_METER_AVAILABLE + 1 == MEMORY_METER_ITEMCOUNT,
      "MEMORY_METER_AVAILABLE is not the last item in MemoryMeterValues");
   this->curItems = MEMORY_METER_AVAILABLE;

   /* we actually want to show "used + shared + compressed" */
   double used = this->values[MEMORY_METER_USED];
   if (isPositive(this->values[MEMORY_METER_SHARED]))
      used += this->values[MEMORY_METER_SHARED];
   if (isPositive(this->values[MEMORY_METER_COMPRESSED]))
      used += this->values[MEMORY_METER_COMPRESSED];

   written = Meter_humanUnit(buffer, used, size);
   METER_BUFFER_CHECK(buffer, size, written);

   METER_BUFFER_APPEND_CHR(buffer, size, '/');

   Meter_humanUnit(buffer, this->total, size);
}

static void MemoryMeter_display(const Object *cast, RichString *out)
{
   char buffer[50] = {0}; // buffer를 한 번 초기화
   const Meter *this = (const Meter *)cast;

   // 출력할 항목의 레이블과 값 배열
   const char *labels[] = {" used:", " shared:", " compressed:", " buffers:", " cache:"};
   const double values[] = {
       this->values[MEMORY_METER_USED],
       this->values[MEMORY_METER_SHARED],
       this->values[MEMORY_METER_COMPRESSED],
       this->values[MEMORY_METER_BUFFERS],
       this->values[MEMORY_METER_CACHE]};

   // 총 메모리 출력
   Meter_humanUnit(buffer, this->total, sizeof(buffer));
   RichString_appendAscii(out, CRT_colors[METER_TEXT], ":");
   RichString_appendAscii(out, CRT_colors[METER_VALUE], buffer);

   // 메모리 속성별 반복 처리
   for (int i = 0; i < 5; i++)
   {
      if (i >= 1 && !isNonnegative(values[i]))
         continue; // 유효하지 않으면 건너뛈다
      Meter_humanUnit(buffer, values[i], sizeof(buffer));
      RichString_appendAscii(out, CRT_colors[METER_TEXT], labels[i]);
      RichString_appendAscii(out, CRT_colors[METER_VALUE], buffer);
   }

   // 사용 가능한 메모리 출력
   if (isNonnegative(this->values[MEMORY_METER_AVAILABLE]))
   {
      Meter_humanUnit(buffer, this->values[MEMORY_METER_AVAILABLE], sizeof(buffer));
      RichString_appendAscii(out, CRT_colors[METER_TEXT], " available:");
      RichString_appendAscii(out, CRT_colors[METER_VALUE], buffer);
   }
}

const MeterClass MemoryMeter_class = {
   .super = {
      .extends = Class(Meter),
      .delete = Meter_delete,
      .display = MemoryMeter_display,
   },
   .updateValues = MemoryMeter_updateValues,
   .defaultMode = BAR_METERMODE,
   .supportedModes = METERMODE_DEFAULT_SUPPORTED,
   .maxItems = MEMORY_METER_ITEMCOUNT,
   .total = 100.0,
   .attributes = MemoryMeter_attributes,
   .name = "Memory",
   .uiName = "Memory",
   .caption = "Mem"
};
