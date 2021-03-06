#pragma once
#include "common.hpp"
#include "pods/candle.hpp"

namespace program::labelling
{
    inline void label_data(std::vector<std::unique_ptr<candle>>* candles)
    {
        std::vector<double> cum_down;
        std::vector<double> cum_up;

        std::vector<double> cum_down_buy;
        std::vector<double> cum_up_buy;

        std::vector<double> cum_down_sell;
        std::vector<double> cum_up_sell;

        double min_change = 0.01;

        double last_max = 0;
        bool allow_buy = false;
        int last_max_index = 0;

        double last_min = 0;
        bool allow_sell = false;
        int last_min_index = 0;

        for (size_t i = 0; i < candles->size(); i++)
        {
            std::unique_ptr<candle>& candle = candles->at(i);

            if (cum_up.size() && cum_down.size())
            {
                double prev_up = cum_up[cum_up.size() - 1];
                double prev_down = cum_down[cum_down.size() - 1];

                double prev_up_buy = cum_up_buy[cum_up_buy.size() - 1];
                double prev_down_buy = cum_down_buy[cum_down_buy.size() - 1];

                double prev_up_sell = cum_up_sell[cum_up_sell.size() - 1];
                double prev_down_sell = cum_down_sell[cum_down_sell.size() - 1];

                // g_log->info("Labelling", "down %lf%", prev_down);
                // g_log->info("Labelling", "up %lf%", prev_up);

                if (candle->m_close > 0)
                {
                    if (prev_up - prev_down > min_change)
                    {
                        if (prev_up_sell - prev_down_sell > last_max)
                        {
                            // g_log->info("Labelling", "new lastmax.");
                            last_max_index = i;
                            last_max = prev_up_sell - prev_down_sell;
                            allow_buy = true;
                            cum_up_buy.clear();
                            cum_down_buy.clear();
                            cum_up.clear();
                            cum_down.clear();
                        }

                        if (allow_sell)
                        {
                            candles->at(last_min_index)->m_target = eTarget::SELL;
                            last_min_index = 0;
                            last_min = min_change;
                            allow_sell = false;
                            cum_up_buy.clear();
                            cum_down_buy.clear();
                        }

                        // cum_up.clear();
                        // cum_down.clear();
                    }
                }
                else if (candle->m_close < 0)
                {
                    if (prev_down - prev_up > min_change)
                    {
                        // g_log->info("Labelling", "candle total down more than 1%.");

                        if (allow_buy)
                        {
                            candles->at(last_max_index)->m_target = eTarget::BUY;
                            last_max_index = 0;
                            last_max = min_change;
                            allow_buy = false;
                            cum_down_sell.clear();
                            cum_up_sell.clear();
                        }

                        if (prev_down_buy - prev_up_buy > last_min)
                        {
                            last_min_index = i;
                            last_min = prev_down_buy - prev_up_buy;
                            allow_sell = true;
                            cum_down_sell.clear();
                            cum_up_sell.clear();
                            cum_up.clear();
                            cum_down.clear();
                        }

                        // set buy and sell

                        // cum_up.clear();
                        // cum_down.clear();
                    }

                }
            }

            if (candle->m_close > 0)
            {
                double prev = cum_up.size() ? cum_up[cum_up.size() - 1] : 0;

                cum_up.push_back(prev + candle->m_close);
                cum_up_buy.push_back(prev + candle->m_close);
                cum_up_sell.push_back(prev + candle->m_close);
            }
            else
            {
                double prev = cum_down.size() ? cum_down[cum_down.size() - 1] : 0;

                cum_down.push_back(prev + fabs(candle->m_close));
                cum_down_buy.push_back(prev + fabs(candle->m_close));
                cum_down_sell.push_back(prev + fabs(candle->m_close));
            }
        }
    }
}