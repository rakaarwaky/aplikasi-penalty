# FITUR.md — Plan to Score 10/10

## Current State (8/10)

**What we have:**
- Registration (5-7 participants)
- Scoring (7 kicks, zone 0-5)
- Ranking (tie-breaker zone 5→1)
- Search (case-insensitive)
- Recap (full table)

**What we're missing:**
- Persistence (save/load data)
- Export (CSV, JSON)
- Undo/redo
- Configuration
- Multiplayer support

## Target State (10/10)

**Enterprise Feature Requirements:**
- Data persistence (save/load)
- Export capabilities
- Undo/redo functionality
- Configuration options
- Multiplayer support
- Plugin system

## Action Items

### 1. Add Data Persistence (High Priority)

**File: `src/shared/infrastructure_storage.h`**

```c
typedef enum {
    STORAGE_OK,
    STORAGE_ERROR_FILE_NOT_FOUND,
    STORAGE_ERROR_PERMISSION,
    STORAGE_ERROR_CORRUPT,
    STORAGE_ERROR_FULL
} StorageError;

typedef struct {
    CompetitionState state;
    char filename[256];
    timestamp_t last_saved;
} Storage;

// Initialize storage
StorageError storage_init(Storage *storage, const char *filename);

// Save state
StorageError storage_save(const Storage *storage);

// Load state
StorageError storage_load(Storage *storage);

// Auto-save
StorageError storage_auto_save(Storage *storage);

// Backup
StorageError storage_backup(const Storage *storage, const char *backup_dir);
```

**Implementation:**
```c
// File: src/shared/infrastructure_storage.c
#include <stdio.h>
#include <string.h>

StorageError storage_init(Storage *storage, const char *filename) {
    if (storage == NULL || filename == NULL) {
        return STORAGE_ERROR_FILE_NOT_FOUND;
    }
    
    strncpy(storage->filename, filename, sizeof(storage->filename) - 1);
    storage->filename[sizeof(storage->filename) - 1] = '\0';
    storage->last_saved = 0;
    
    // Try to load existing file
    return storage_load(storage);
}

StorageError storage_save(const Storage *storage) {
    if (storage == NULL) {
        return STORAGE_ERROR_FILE_NOT_FOUND;
    }
    
    FILE *file = fopen(storage->filename, "wb");
    if (file == NULL) {
        return STORAGE_ERROR_PERMISSION;
    }
    
    // Write state
    size_t written = fwrite(&storage->state, sizeof(CompetitionState), 1, file);
    fclose(file);
    
    if (written != 1) {
        return STORAGE_ERROR_CORRUPT;
    }
    
    return STORAGE_OK;
}

StorageError storage_load(Storage *storage) {
    if (storage == NULL) {
        return STORAGE_ERROR_FILE_NOT_FOUND;
    }
    
    FILE *file = fopen(storage->filename, "rb");
    if (file == NULL) {
        return STORAGE_ERROR_FILE_NOT_FOUND;
    }
    
    // Read state
    size_t read = fread(&storage->state, sizeof(CompetitionState), 1, file);
    fclose(file);
    
    if (read != 1) {
        return STORAGE_ERROR_CORRUPT;
    }
    
    return STORAGE_OK;
}
```

### 2. Add Export Capabilities (High Priority)

**File: `src/shared/infrastructure_export.h`**

```c
typedef enum {
    EXPORT_OK,
    EXPORT_ERROR_FILE_NOT_FOUND,
    EXPORT_ERROR_PERMISSION,
    EXPORT_ERROR_FORMAT
} ExportError;

typedef enum {
    EXPORT_CSV,
    EXPORT_JSON,
    EXPORT_XML,
    EXPORT_HTML
} ExportFormat;

// Export ranking
ExportError export_ranking(const CompetitionState *state,
                          const char *filename,
                          ExportFormat format);

// Export recap
ExportError export_recap(const CompetitionState *state,
                        const char *filename,
                        ExportFormat format);

// Export all data
ExportError export_all(const CompetitionState *state,
                      const char *filename,
                      ExportFormat format);
```

**Implementation:**
```c
// File: src/shared/infrastructure_export.c
#include <stdio.h>
#include <string.h>

ExportError export_ranking(const CompetitionState *state,
                          const char *filename,
                          ExportFormat format) {
    if (state == NULL || filename == NULL) {
        return EXPORT_ERROR_FILE_NOT_FOUND;
    }
    
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        return EXPORT_ERROR_PERMISSION;
    }
    
    switch (format) {
        case EXPORT_CSV:
            fprintf(file, "Rank,Name,Score\n");
            // ... write CSV data
            break;
            
        case EXPORT_JSON:
            fprintf(file, "[\n");
            // ... write JSON data
            fprintf(file, "]\n");
            break;
            
        default:
            fclose(file);
            return EXPORT_ERROR_FORMAT;
    }
    
    fclose(file);
    return EXPORT_OK;
}
```

### 3. Add Undo/Redo (Medium Priority)

**File: `src/shared/infrastructure_undo.h`**

```c
typedef enum {
    ACTION_REGISTER,
    ACTION_SCORE,
    ACTION_RANKING
} ActionType;

typedef struct {
    ActionType type;
    CompetitionState before;
    CompetitionState after;
    timestamp_t timestamp;
} Action;

typedef struct {
    Action actions[100];
    int count;
    int current;
} UndoRedo;

// Initialize undo/redo
void undo_redo_init(UndoRedo *ur);

// Record action
void undo_redo_record(UndoRedo *ur, ActionType type,
                     const CompetitionState *before,
                     const CompetitionState *after);

// Undo
UndoError undo_redo_undo(UndoRedo *ur, CompetitionState *state);

// Redo
UndoError undo_redo_redo(UndoRedo *ur, CompetitionState *state);

// Get history
UndoError undo_redo_history(const UndoRedo *ur, char *buffer, size_t size);
```

### 4. Add Configuration (Medium Priority)

**File: `src/shared/infrastructure_config.h`**

```c
typedef struct {
    int min_participants;
    int max_participants;
    int kicks_per_player;
    int min_zone;
    int max_zone;
    char title[100];
    char save_dir[256];
    char export_dir[256];
    int auto_save_interval;
    int enable_logging;
    LogLevel log_level;
} Config;

// Initialize config with defaults
void config_init(Config *config);

// Load config from file
ConfigError config_load(Config *config, const char *filename);

// Save config to file
ConfigError config_save(const Config *config, const char *filename);

// Get config value
ConfigError config_get(const Config *config, const char *key,
                      char *value, size_t value_size);

// Set config value
ConfigError config_set(Config *config, const char *key,
                      const char *value);
```

### 5. Add Multiplayer Support (Low Priority)

**File: `src/shared/infrastructure_multiplayer.h`**

```c
typedef struct {
    char player_name[30];
    int score;
    int kicks[7];
} Player;

typedef struct {
    Player players[100];
    int count;
    int current_player;
    int round;
} MultiplayerGame;

// Initialize multiplayer game
MultiplayerError multiplayer_init(MultiplayerGame *game, int num_players);

// Add player
MultiplayerError multiplayer_add_player(MultiplayerGame *game,
                                       const char *name);

// Record kick
MultiplayerError multiplayer_record_kick(MultiplayerGame *game,
                                        int player_index,
                                        int kick_index,
                                        int zone);

// Get rankings
MultiplayerError multiplayer_get_rankings(const MultiplayerGame *game,
                                         Player *rankings,
                                         int capacity);

// Get current player
MultiplayerError multiplayer_get_current_player(const MultiplayerGame *game,
                                               Player **player);
```

### 6. Add Plugin System (Low Priority)

**File: `src/shared/infrastructure_plugin.h`**

```c
typedef enum {
    PLUGIN_OK,
    PLUGIN_ERROR_NOT_FOUND,
    PLUGIN_ERROR_LOAD_FAILED,
    PLUGIN_ERROR_SYMBOL_NOT_FOUND
} PluginError;

typedef struct {
    char name[100];
    char version[50];
    void *handle;
    void (*init)(void);
    void (*cleanup)(void);
    int (*execute)(void *data);
} Plugin;

// Load plugin
PluginError plugin_load(Plugin *plugin, const char *filename);

// Unload plugin
PluginError plugin_unload(Plugin *plugin);

// Execute plugin
PluginError plugin_execute(const Plugin *plugin, void *data);

// List plugins
PluginError plugin_list(char *buffer, size_t size);
```

## Feature Roadmap

| Phase | Features | Duration |
|-------|----------|----------|
| Phase 1 | Persistence + Export | 2 weeks |
| Phase 2 | Undo/Redo + Config | 2 weeks |
| Phase 3 | Multiplayer + Plugin | 3 weeks |

## Success Criteria

- [ ] Data persistence working
- [ ] Export to CSV/JSON working
- [ ] Undo/redo functional
- [ ] Configuration system working
- [ ] Multiplayer support working
- [ ] Plugin system working
