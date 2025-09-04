#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include "clock.h"

int main()
{
  std::cout << "CaravanOS Hello (Linux)\n";
  std::unique_ptr<IClock> clk = hal_make_clock();
  if (clk)
  {
    std::cout << "millis64() = " << clk->millis64() << " ms\n";
  }
  else
  {
    std::cout << "No clock factory available.\n";
  }

  std::this_thread::sleep_for(std::chrono::seconds(1));
  std::cout << "millis64() after 1s sleep: " << clk->millis64() << " ms\n";
  return 0;
}
