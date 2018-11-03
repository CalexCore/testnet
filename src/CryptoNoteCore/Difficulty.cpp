// Copyright (c) 2018, The TurtleCoin Developers
// Copyright (c) 2018, The Calex Developers
// 
// Please see the included LICENSE file for more information.

#include <algorithm>

#include "Difficulty.h"

#include <config/CryptoNoteConfig.h>

// LWMA-4 difficulty algorithm
// Copyright (c) 2017-2018 Zawy, MIT License
// https://github.com/zawy12/difficulty-algorithms/issues/3
uint64_t nextDifficultyV7(std::vector<uint64_t> timestamps, std::vector<uint64_t> cumulativeDifficulties)
{
    uint64_t T = CryptoNote::parameters::DIFFICULTY_TARGET;
    uint64_t N = CryptoNote::parameters::DIFFICULTY_WINDOW_V3;
    uint64_t L(0), ST, next_D, prev_D, avg_D, i;

    /* If we are starting up, returning a difficulty guess. If you are a
       new coin, you might want to set this to a decent estimate of your
       hashrate */
    if (timestamps.size() < static_cast<uint64_t>(N+1))
    {
        return 5000;
    }
	
    for ( i = 1; i <= N; i++) {        
	if ( timestamps[i] > timestamps[i-1] ) { timestamps[i] = timestamps[i]; } 
	else { timestamps[i] = timestamps[i-1]; }
   }

	
    for (int64_t i = 1; i <= N; i++) {
    if ( i > 4 && timestamps[i]-timestamps[i-1] > 4*T  && timestamps[i-1] - timestamps[i-4] < (16*T)/10 ) { ST = T; }
	else if ( i > 7 && timestamps[i]-timestamps[i-1] > 4*T  && timestamps[i-1] - timestamps[i-7] < 4*T ) { ST = T; }
	else { // Assume normal conditions, so get ST.
         // LWMA drops too much from long ST, so limit drops with a 5*T limit 
	ST = std::min(5*T ,timestamps[i] - timestamps[i-1]);
    }
    L +=  ST * i ;
  }


   if (L < N*(N+1)*T/4 ) { L =  N*(N+1)*T/4; }

   avg_D = ( cumulativeDifficulties[N] - cumulativeDifficulties[0] )/ N;
   next_D = (avg_D*T*N*(N+1)*97)/(100*2*L);
   prev_D =  cumulativeDifficulties[N] - cumulativeDifficulties[N-1] ;

   // Apply 10% jump rule.
   if (  ( timestamps[N] - timestamps[N-3] < (8*T)/10 ) || (timestamps[N] - timestamps[N-2] < (5*T)/10) || (timestamps[N] - timestamps[N-1] < (2*T)/10) )
   {
       next_D = std::max( next_D, std::min( (prev_D*110)/100, (105*avg_D)/100 ) );
   }
   
   // Make all insignificant digits zero for easy reading.
   i = 1000000000;
   while (i > 1) {
     if ( next_D > i*100 ) { next_D = ((next_D+i/2)/i)*i; break; }
     else { i /= 10; }
   }
   
   // Make least 3 digits equal avg of past 10 solvetimes.
   if ( next_D > 100000 ) {
    next_D = ((next_D+500)/1000)*1000 + std::min(static_cast<uint64_t>(999), (timestamps[N]-timestamps[N-10])/10);
   }
   
   return  next_D;
}

// LWMA-3 difficulty algorithm
// Copyright (c) 2017-2018 Zawy, MIT License
// https://github.com/zawy12/difficulty-algorithms/issues/3
uint64_t nextDifficultyV6(std::vector<uint64_t> timestamps, std::vector<uint64_t> cumulativeDifficulties)
{
    uint64_t T = CryptoNote::parameters::DIFFICULTY_TARGET;
    uint64_t N = CryptoNote::parameters::DIFFICULTY_WINDOW_V3;
    uint64_t L(0), ST, sum_3_ST(0), next_D, prev_D, nextTime, lastTime;

	
    /* If we are starting up, returning a difficulty guess. If you are a
       new coin, you might want to set this to a decent estimate of your
       hashrate */
    if (timestamps.size() < static_cast<uint64_t>(N+1))
    {
        return 5000;
    }
	
	lastTime = timestamps[0];
	
    for ( int64_t i = 1; i <= N; i++) {
      if (static_cast<int64_t>(timestamps[i]) > lastTime) {
      nextTime = timestamps[i];
      } else { nextTime = lastTime+1; }
      ST = std::min(6*T,nextTime - lastTime);
      lastTime = nextTime;
      L +=  ST * i;
      if ( i > N-3 ) { sum_3_ST += ST; }
    }

    next_D = (static_cast<int64_t>(cumulativeDifficulties[N] - cumulativeDifficulties[0]) * T * (N+1) * 99) / (100 * 2 * L);

    prev_D = cumulativeDifficulties[N] - cumulativeDifficulties[N-1];

    next_D = std::max((prev_D * 67) / 100, std::min(next_D, (prev_D * 150) / 100));

    if (sum_3_ST < (9 * T) / 10)
    {  
        next_D = std::max(next_D, (prev_D * 108) / 100);
    }

    return static_cast<uint64_t>(next_D);
}

// LWMA-2 difficulty algorithm 
// Copyright (c) 2017-2018 Zawy, MIT License
// https://github.com/zawy12/difficulty-algorithms/issues/3
uint64_t nextDifficultyV5(std::vector<uint64_t> timestamps, std::vector<uint64_t> cumulativeDifficulties)
{
    int64_t T = CryptoNote::parameters::DIFFICULTY_TARGET;
    int64_t N = CryptoNote::parameters::DIFFICULTY_WINDOW_V3;
    int64_t L(0), ST, sum_3_ST(0), next_D, prev_D;

    /* If we are starting up, returning a difficulty guess. If you are a
       new coin, you might want to set this to a decent estimate of your
       hashrate */
    if (timestamps.size() < static_cast<uint64_t>(N+1))
    {
        return 5000;
    }

    for (int64_t i = 1; i <= N; i++)
    {  
      ST = static_cast<int64_t>(timestamps[i]) - static_cast<int64_t>(timestamps[i-1]);

      ST = std::max(-4 * T, std::min(ST, 6 * T));

      L +=  ST * i ;

      if ( i > N-3 ) { sum_3_ST += ST; } 
    }

    next_D = (static_cast<int64_t>(cumulativeDifficulties[N] - cumulativeDifficulties[0]) * T * (N+1) * 99) / (100 * 2 * L);

    prev_D = cumulativeDifficulties[N] - cumulativeDifficulties[N-1];

    next_D = std::max((prev_D * 67) / 100, std::min(next_D, (prev_D * 150) / 100));

    if (sum_3_ST < (8 * T) / 10)
    {  
        next_D = std::max(next_D, (prev_D * 108) / 100);
    }

    return static_cast<uint64_t>(next_D);
}

// LWMA-2 difficulty algorithm 
// Copyright (c) 2017-2018 Zawy, MIT License
// https://github.com/zawy12/difficulty-algorithms/issues/3
uint64_t nextDifficultyV4(std::vector<uint64_t> timestamps, std::vector<uint64_t> cumulativeDifficulties)
{
    int64_t T = CryptoNote::parameters::DIFFICULTY_TARGET;
    int64_t N = CryptoNote::parameters::DIFFICULTY_WINDOW_V3;
    int64_t L(0), ST, sum_3_ST(0), next_D, prev_D;

    if (timestamps.size() <= static_cast<uint64_t>(N))
    {
        return 5000;
    }

    for (int64_t i = 1; i <= N; i++)
    {  
        ST = clamp(-6 * T, static_cast<int64_t>(timestamps[i]) - static_cast<int64_t>(timestamps[i-1]), 6 * T);

        L +=  ST * i; 

        if (i > N-3)
        {
            sum_3_ST += ST;
        } 
    }

    next_D = (static_cast<int64_t>(cumulativeDifficulties[N] - cumulativeDifficulties[0]) * T * (N+1) * 99) / (100 * 2 * L);
    prev_D = cumulativeDifficulties[N] - cumulativeDifficulties[N-1];

    /* Make sure we don't divide by zero if 50x attacker (thanks fireice) */
    next_D = std::max((prev_D*67)/100, std::min(next_D, (prev_D*150)/100));

    if (sum_3_ST < (8 * T) / 10)
    {  
        next_D = std::max(next_D, (prev_D * 110) / 100);
    }

    return static_cast<uint64_t>(next_D);
}

// LWMA-2 difficulty algorithm 
// Copyright (c) 2017-2018 Zawy, MIT License
// https://github.com/zawy12/difficulty-algorithms/issues/3
uint64_t nextDifficultyV3(std::vector<uint64_t> timestamps, std::vector<uint64_t> cumulativeDifficulties)
{
    int64_t T = CryptoNote::parameters::DIFFICULTY_TARGET;
    int64_t N = CryptoNote::parameters::DIFFICULTY_WINDOW_V3;
    int64_t FTL = CryptoNote::parameters::CRYPTONOTE_BLOCK_FUTURE_TIME_LIMIT_V3;
    int64_t L(0), ST, sum_3_ST(0), next_D, prev_D;

    if (timestamps.size() <= static_cast<uint64_t>(N))
    {
        return 5000;
    }

    for (int64_t i = 1; i <= N; i++)
    {  
        ST = std::max(-FTL, std::min(static_cast<int64_t>(timestamps[i]) - static_cast<int64_t>(timestamps[i-1]), 6 * T));

        L +=  ST * i; 

        if (i > N-3)
        {
            sum_3_ST += ST;
        } 
    }

    next_D = (static_cast<int64_t>(cumulativeDifficulties[N] - cumulativeDifficulties[0]) * T * (N+1) * 99) / (100 * 2 * L);
    prev_D = cumulativeDifficulties[N] - cumulativeDifficulties[N-1];

    /* Make sure we don't divide by zero if 50x attacker (thanks fireice) */
    next_D = std::max((prev_D*70)/100, std::min(next_D, (prev_D*107)/100));

    if (sum_3_ST < (8 * T) / 10)
    {  
        next_D = (prev_D * 110) / 100;
    }

    return static_cast<uint64_t>(next_D);
}
