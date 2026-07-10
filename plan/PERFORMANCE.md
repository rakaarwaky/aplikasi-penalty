# PERFORMANCE.md — Plan to Score 10/10

## Current State (7/10)

**What we have:**
- C99 (fast language)
- Efficient algorithms
- Minimal memory usage
- No unnecessary allocations

**What we're missing:**
- Performance profiling
- Benchmarking
- Memory optimization
- Algorithm optimization
- Load testing

## Target State (10/10)

**Enterprise Performance Requirements:**
- Performance profiling
- Benchmarking suite
- Memory optimization
- Algorithm optimization
- Load testing
- Performance regression testing

## Action Items

### 1. Add Performance Profiling (High Priority)

**Tool: gprof**

**Setup:**
```bash
# Build with profiling
make CFLAGS="-pg"

# Run program
./aplikasi_perhitungan_penalty

# Generate profile
gprof aplikasi_perhitungan_penalty gmon.out > analysis.txt
```

**Integration:**
```makefile
# Add to Makefile
profile:
    make CFLAGS="-pg" LDFLAGS="-pg"
    ./aplikasi_perhitungan_penalty < /dev/null
    gprof aplikasi_perhitungan_penalty gmon.out > analysis.txt
    @echo "Profile generated: analysis.txt"
```

### 2. Add Benchmarking Suite (High Priority)

**File: `tests/test_benchmark.c`**

```c
#include <time.h>
#include <stdio.h>

typedef struct {
    const char *name;
    double time_seconds;
    int operations;
    double ops_per_second;
} Benchmark;

Benchmark benchmark_registration(int num_participants) {
    clock_t start = clock();
    
    CompetitionState state = {0};
    for (int i = 0; i < num_participants; i++) {
        ParticipantNameVO name = {{0}};
        snprintf(name.value, 10, "Player%d", i);
        capabilities_registration_append(&state, &name);
    }
    
    clock_t end = clock();
    double time_taken = (double)(end - start) / CLOCKS_PER_SEC;
    
    return (Benchmark){
        .name = "Registration",
        .time_seconds = time_taken,
        .operations = num_participants,
        .ops_per_second = num_participants / time_taken
    };
}

Benchmark benchmark_scoring(int num_participants, int kicks_per_player) {
    clock_t start = clock();
    
    CompetitionState state = {0};
    // Register participants first
    for (int i = 0; i < num_participants; i++) {
        ParticipantNameVO name = {{0}};
        snprintf(name.value, 10, "Player%d", i);
        capabilities_registration_append(&state, &name);
    }
    
    // Record kicks
    for (int p = 0; p < num_participants; p++) {
        for (int k = 0; k < kicks_per_player; k++) {
            ZoneVO z = {k % 6};
            capabilities_scoring_record_kick(&state, p, z);
        }
    }
    
    clock_t end = clock();
    double time_taken = (double)(end - start) / CLOCKS_PER_SEC;
    int total_operations = num_participants * kicks_per_player;
    
    return (Benchmark){
        .name = "Scoring",
        .time_seconds = time_taken,
        .operations = total_operations,
        .ops_per_second = total_operations / time_taken
    };
}

Benchmark benchmark_ranking(int num_participants) {
    clock_t start = clock();
    
    CompetitionState state = {0};
    // Setup state with completed scoring
    // ... (similar to above)
    
    RankingEntryVO entries[MAX_PARTICIPANTS];
    agent_ranking_calculate(&agg, &state, entries, MAX_PARTICIPANTS);
    
    clock_t end = clock();
    double time_taken = (double)(end - start) / CLOCKS_PER_SEC;
    
    return (Benchmark){
        .name = "Ranking",
        .time_seconds = time_taken,
        .operations = 1,
        .ops_per_second = 1.0 / time_taken
    };
}
```

### 3. Add Memory Profiling (High Priority)

**Tool: valgrind**

**Setup:**
```bash
# Run with memory profiling
valgrind --tool=massif --pages-as-heap=no ./run_tests

# Analyze results
ms_print massif.out.*
```

**Integration:**
```makefile
# Add to Makefile
memory-profile:
    valgrind --tool=massif --massif-out-file=massif.out ./run_tests
    ms_print massif.out
    @echo "Memory profile generated"
```

### 4. Add Load Testing (Medium Priority)

**File: `tests/test_load.c`**

```c
void test_load_1000_participants(void) {
    printf("Load test: 1000 participants\n");
    
    clock_t start = clock();
    
    CompetitionState state = {0};
    for (int i = 0; i < 1000; i++) {
        ParticipantNameVO name = {{0}};
        snprintf(name.value, 10, "P%d", i);
        capabilities_registration_append(&state, &name);
    }
    
    // Record kicks for all
    for (int p = 0; p < 1000; p++) {
        for (int k = 0; k < 7; k++) {
            ZoneVO z = {k % 6};
            capabilities_scoring_record_kick(&state, p, z);
        }
    }
    
    // Calculate ranking
    RankingEntryVO entries[1000];
    // ... (calculate ranking)
    
    clock_t end = clock();
    double time_taken = (double)(end - start) / CLOCKS_PER_SEC;
    
    printf("  Time: %.3f seconds\n", time_taken);
    printf("  Participants: 1000\n");
    printf("  Total kicks: 7000\n");
    printf("  Performance: %.0f operations/second\n", 8000 / time_taken);
    
    assert(time_taken < 5.0);  // Should complete in under 5 seconds
    printf("  [PASS] test_load_1000_participants\n");
}
```

### 5. Add Memory Optimization (Medium Priority)

**Optimizations:**
- Use stack allocation where possible
- Minimize heap allocations
- Use memory pools for frequent allocations
- Reduce struct padding

**Example:**
```c
// Before
typedef struct {
    char name[30];
    int score;
    int zone_freq[6];
} Participant;

// After (packed)
typedef struct {
    char name[30];
    int32_t score;
    int16_t zone_freq[6];
} __attribute__((packed)) Participant;
```

### 6. Add Algorithm Optimization (Low Priority)

**Optimizations:**
- Use more efficient sorting algorithms
- Optimize search with hash tables
- Cache computed results
- Use bit operations where possible

**Example:**
```c
// Before: O(n) search
int find_participant(const CompetitionState *state, const char *name) {
    for (int i = 0; i < state->participant_count; i++) {
        if (strcmp(state->participants[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

// After: O(1) hash table lookup
typedef struct {
    char *name;
    int index;
} HashEntry;

typedef struct {
    HashEntry entries[MAX_PARTICIPANTS];
    int count;
} ParticipantHash;

int find_participant_hash(const ParticipantHash *hash, const char *name) {
    unsigned int hash_value = hash_string(name);
    int index = hash_value % MAX_PARTICIPANTS;
    while (hash->entries[index].name != NULL) {
        if (strcmp(hash->entries[index].name, name) == 0) {
            return hash->entries[index].index;
        }
        index = (index + 1) % MAX_PARTICIPANTS;
    }
    return -1;
}
```

## Performance Targets

| Metric | Current | Target |
|--------|---------|--------|
| Registration (1000 participants) | ~10ms | <5ms |
| Scoring (7000 kicks) | ~50ms | <20ms |
| Ranking (1000 participants) | ~5ms | <2ms |
| Search (1000 participants) | ~1ms | <0.5ms |
| Memory usage (1000 participants) | ~100KB | <50KB |

## Implementation Steps

1. **Week 1:** Add profiling and benchmarking
2. **Week 2:** Add memory profiling and optimization
3. **Week 3:** Add load testing
4. **Week 4:** Add algorithm optimization

## Success Criteria

- [ ] All benchmarks within targets
- [ ] No memory leaks
- [ ] Performance regression tests passing
- [ ] Memory usage optimized
- [ ] Algorithms optimized
