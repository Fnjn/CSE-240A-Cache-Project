//========================================================//
//  cache.c                                               //
//  Source file for the Cache Simulator                   //
//                                                        //
//  Implement the I-cache, D-Cache and L2-cache as        //
//  described in the README                               //
//========================================================//

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "cache.h"

//
// TODO:Student Information
//
const char *studentName = "Fanjin Zeng";
const char *studentID   = "A53238021";
const char *email       = "f1zeng@ucsd.edu";

//------------------------------------//
//        Cache Configuration         //
//------------------------------------//

uint32_t icacheSets;     // Number of sets in the I$
uint32_t icacheAssoc;    // Associativity of the I$
uint32_t icacheHitTime;  // Hit Time of the I$

uint32_t dcacheSets;     // Number of sets in the D$
uint32_t dcacheAssoc;    // Associativity of the D$
uint32_t dcacheHitTime;  // Hit Time of the D$

uint32_t l2cacheSets;    // Number of sets in the L2$
uint32_t l2cacheAssoc;   // Associativity of the L2$
uint32_t l2cacheHitTime; // Hit Time of the L2$
uint32_t inclusive;      // Indicates if the L2 is inclusive

uint32_t blocksize;      // Block/Line size
uint32_t memspeed;       // Latency of Main Memory

//------------------------------------//
//          Cache Statistics          //
//------------------------------------//

uint64_t icacheRefs;       // I$ references
uint64_t icacheMisses;     // I$ misses
uint64_t icachePenalties;  // I$ penalties

uint64_t dcacheRefs;       // D$ references
uint64_t dcacheMisses;     // D$ misses
uint64_t dcachePenalties;  // D$ penalties

uint64_t l2cacheRefs;      // L2$ references
uint64_t l2cacheMisses;    // L2$ misses
uint64_t l2cachePenalties; // L2$ penalties

//------------------------------------//
//        Cache Data Structures       //
//------------------------------------//

//
//TODO: Add your Cache data structures here
//

uint32_t **icache;
uint32_t **dcache;
uint32_t **l2cache;

uint32_t offset_size;
uint32_t offset_mask;
uint32_t index_mask;

// int log2(int n)
// {
//   return log(n) / log(2);
// }

//------------------------------------//
//          Cache Functions           //
//------------------------------------//

// Initialize the Cache Hierarchy
//
void
init_cache()
{
  // Initialize cache stats
  icacheRefs        = 0;
  icacheMisses      = 0;
  icachePenalties   = 0;
  dcacheRefs        = 0;
  dcacheMisses      = 0;
  dcachePenalties   = 0;
  l2cacheRefs       = 0;
  l2cacheMisses     = 0;
  l2cachePenalties  = 0;

  icache = (uint32_t**)malloc(sizeof(uint32_t*) * icacheSets);
  dcache = (uint32_t**)malloc(sizeof(uint32_t*) * dcacheSets);
  l2cache = (uint32_t**)malloc(sizeof(uint32_t*) * l2cacheSets);

  for(int i=0; i<icacheSets; i++)
  {
    icache[i] = (uint32_t*)malloc(sizeof(uint32_t) * icacheAssoc);
    for(int j=0; j<icacheAssoc; j++)
      icache[i][j] = 0;
    }

  for(int i=0; i<dcacheSets; i++)
  {
    dcache[i] = (uint32_t*)malloc(sizeof(uint32_t) * dcacheAssoc);
    for(int j=0; j<dcacheAssoc; j++)
      dcache[i][j] = 0;
    }

  for(int i=0; i<l2cacheSets; i++)
  {
    l2cache[i] = (uint32_t*)malloc(sizeof(uint32_t) * l2cacheAssoc);
    for(int j=0; j<l2cacheAssoc; j++)
      l2cache[i][j] = 0;
    }

    offset_size = (uint32_t)log2(blocksize);
    offset_mask = (1 << offset_size) - 1;
}

// Perform a memory access through the icache interface for the address 'addr'
// Return the access time for the memory operation
//
uint32_t
icache_access(uint32_t addr)
{
  icacheRefs += 1;

  uint32_t index_mask = (1 << icacheSets) - 1;
  uint32_t offset = addr & offset_mask;
  uint32_t index = (addr >> offset_size) & index_mask;
  uint32_t tag = addr >> (icacheSets + offset_size);

  for(int i=0; i<icacheAssoc; i++){
    if(icache[index][i] == tag){ // Hit
      return icacheHitTime;
    }
  }

  icacheMisses += 1;
  icache[index][rand()%icacheAssoc] = tag;

  uint32_t penalty = l2cache_access(addr);
  icachePenalties += penalty;

  return penalty + icacheHitTime;
}

// Perform a memory access through the dcache interface for the address 'addr'
// Return the access time for the memory operation
//
uint32_t
dcache_access(uint32_t addr)
{
  dcacheRefs += 1;

  uint32_t index_mask = (1 << dcacheSets) - 1;
  uint32_t offset = addr & offset_mask;
  uint32_t index = (addr >> offset_size) & index_mask;
  uint32_t tag = addr >> (dcacheSets + offset_size);

  for(int i=0; i<dcacheAssoc; i++){
    if(dcache[index][i] == tag){ // Hit
      return dcacheHitTime;
    }
  }

  dcacheMisses += 1;
  dcache[index][rand()%dcacheAssoc] = tag;

  uint32_t penalty = l2cache_access(addr);
  dcachePenalties += penalty;

  return penalty + dcacheHitTime;
}

// Perform a memory access to the l2cache for the address 'addr'
// Return the access time for the memory operation
//
uint32_t
l2cache_access(uint32_t addr)
{
  l2cacheRefs += 1;

  uint32_t index_mask = (1 << dcacheSets) - 1;
  uint32_t offset = addr & offset_mask;
  uint32_t index = (addr >> offset_size) & index_mask;
  uint32_t tag = addr >> (l2cacheSets + offset_size);

  for(int i=0; i<l2cacheAssoc; i++){
    if(l2cache[index][i] == tag){ // Hit
      return l2cacheHitTime;
    }
  }

  l2cacheMisses += 1;
  l2cache[index][rand()%l2cacheAssoc] = tag;

  l2cachePenalties += memspeed;
  return memspeed + l2cacheHitTime;
}
