#include "KeyboardLayoutPresets.h"

#include <algorithm>
#include <array>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <windows.h>

namespace keyviz
{
namespace
{
constexpr KeyBinding kTopRowKeys[] =
{
    { "TAB", VK_TAB, 0.96f },
    { "Q", 'Q', 0.86f },
    { "W", 'W', 0.86f },
    { "E", 'E', 0.86f },
    { "R", 'R', 0.86f },
};

constexpr KeyBinding kMiddleRowKeys[] =
{
    { "SHIFT", VK_SHIFT, 1.18f },
    { "A", 'A', 0.86f },
    { "S", 'S', 0.86f },
    { "D", 'D', 0.86f },
    { "F", 'F', 0.86f },
};

constexpr KeyBinding kBottomRowKeys[] =
{
    { "CTRL", VK_CONTROL, 1.02f },
    { "", VK_SPACE, 3.14f },
};

constexpr KeyBinding kMouseRowKeys[] =
{
    { "", VK_LBUTTON, 1.00f },
    { "", VK_MBUTTON, 1.00f },
    { "", VK_RBUTTON, 1.00f },
};

constexpr const char* kLayoutPresetLabels[] =
{
    "Keyboard Only",
    "Keyboard + Mouse",
    "Custom",
};

constexpr std::size_t kTopRowKeyCount = sizeof(kTopRowKeys) / sizeof(kTopRowKeys[0]);
constexpr std::size_t kMiddleRowKeyCount = sizeof(kMiddleRowKeys) / sizeof(kMiddleRowKeys[0]);
constexpr std::size_t kBottomRowKeyCount = sizeof(kBottomRowKeys) / sizeof(kBottomRowKeys[0]);
constexpr std::size_t kMouseRowKeyCount = sizeof(kMouseRowKeys) / sizeof(kMouseRowKeys[0]);

constexpr std::array<KeyRow, 3> kKeyboardRows =
{
    KeyRow{ kTopRowKeys, kTopRowKeyCount },
    KeyRow{ kMiddleRowKeys, kMiddleRowKeyCount },
    KeyRow{ kBottomRowKeys, kBottomRowKeyCount },
};

constexpr std::array<KeyRow, 4> kKeyboardMouseRows =
{
    KeyRow{ kTopRowKeys, kTopRowKeyCount },
    KeyRow{ kMiddleRowKeys, kMiddleRowKeyCount },
    KeyRow{ kBottomRowKeys, kBottomRowKeyCount },
    KeyRow{ kMouseRowKeys, kMouseRowKeyCount },
};

constexpr int kCustomPresetIndex = 2;
constexpr const char* kCustomLayoutFilePath = "config/custom_layout.txt";
constexpr const char* kCustomLayoutExchangePath = "config/custom_layout_exchange.txt";
constexpr std::size_t kMaxCustomRowKeyCount = 12U;
constexpr std::size_t kMinCustomRowCount = 1U;
constexpr std::size_t kMaxCustomRowCount = 8U;

constexpr KeyBinding kPaletteKeys[] =
{
    { "ESC", VK_ESCAPE, 0.92f },
    { "TAB", VK_TAB, 0.96f },
    { "CAPS", VK_CAPITAL, 1.06f },
    { "SHIFT", VK_SHIFT, 1.18f },
    { "CTRL", VK_CONTROL, 1.02f },
    { "ALT", VK_MENU, 1.02f },
    { "SPACE", VK_SPACE, 3.14f },
    { "Q", 'Q', 0.86f },
    { "W", 'W', 0.86f },
    { "E", 'E', 0.86f },
    { "R", 'R', 0.86f },
    { "T", 'T', 0.86f },
    { "A", 'A', 0.86f },
    { "S", 'S', 0.86f },
    { "D", 'D', 0.86f },
    { "F", 'F', 0.86f },
    { "G", 'G', 0.86f },
    { "Z", 'Z', 0.86f },
    { "X", 'X', 0.86f },
    { "C", 'C', 0.86f },
    { "V", 'V', 0.86f },
    { "B", 'B', 0.86f },
    { "1", '1', 0.86f },
    { "2", '2', 0.86f },
    { "3", '3', 0.86f },
    { "4", '4', 0.86f },
};

struct CustomLayoutStorage
{
    std::vector<std::vector<KeyBinding>> rows{};
    std::vector<KeyRow> rowViews{};
    std::vector<std::string> rowLabelStorage{};
    std::vector<const char*> rowLabelPtrs{};
    std::array<const char*, sizeof(kPaletteKeys) / sizeof(kPaletteKeys[0])> paletteLabels{};
    bool includeMouse = true;
    bool initialized = false;
};

CustomLayoutStorage& GetCustomStorage()
{
    static CustomLayoutStorage storage{};
    return storage;
}

const KeyBinding* FindPaletteKeyByCode(std::uint32_t keyCode)
{
    for (const KeyBinding& key : kPaletteKeys)
    {
        if (key.keyCode == keyCode)
        {
            return &key;
        }
    }

    return nullptr;
}

bool IsEditableRowIndex(const CustomLayoutStorage& storage, int rowIndex)
{
    return rowIndex >= 0 && static_cast<std::size_t>(rowIndex) < storage.rows.size();
}

void RefreshRowLabels(CustomLayoutStorage& storage)
{
    storage.rowLabelStorage.clear();
    storage.rowLabelPtrs.clear();
    storage.rowLabelStorage.reserve(storage.rows.size());
    storage.rowLabelPtrs.reserve(storage.rows.size());

    for (std::size_t rowIndex = 0; rowIndex < storage.rows.size(); ++rowIndex)
    {
        storage.rowLabelStorage.push_back("Row " + std::to_string(rowIndex + 1U));
        storage.rowLabelPtrs.push_back(storage.rowLabelStorage.back().c_str());
    }
}

void ResetCustomLayoutStorage(CustomLayoutStorage& storage)
{
    storage.rows.clear();
    storage.rows.resize(3U);
    storage.rows[0].assign(kTopRowKeys, kTopRowKeys + kTopRowKeyCount);
    storage.rows[1].assign(kMiddleRowKeys, kMiddleRowKeys + kMiddleRowKeyCount);
    storage.rows[2].assign(kBottomRowKeys, kBottomRowKeys + kBottomRowKeyCount);
    storage.includeMouse = true;
    RefreshRowLabels(storage);
}

void NormalizeCustomLayoutStorage(CustomLayoutStorage& storage)
{
    if (storage.rows.empty())
    {
        storage.rows.resize(kMinCustomRowCount);
    }
    if (storage.rows.size() > kMaxCustomRowCount)
    {
        storage.rows.resize(kMaxCustomRowCount);
    }
    for (std::vector<KeyBinding>& row : storage.rows)
    {
        if (row.size() > kMaxCustomRowKeyCount)
        {
            row.resize(kMaxCustomRowKeyCount);
        }
    }
    RefreshRowLabels(storage);
}

bool SaveCustomLayoutToPath(const CustomLayoutStorage& storage, const char* path)
{
    if (path == nullptr || path[0] == '\0')
    {
        return false;
    }

    std::filesystem::path targetPath(path);
    if (targetPath.has_parent_path())
    {
        std::error_code errorCode;
        std::filesystem::create_directories(targetPath.parent_path(), errorCode);
    }

    std::ofstream output(path, std::ios::trunc);
    if (!output.is_open())
    {
        return false;
    }

    output << "version 2\n";
    output << "mouse " << (storage.includeMouse ? 1 : 0) << '\n';
    output << "rows " << storage.rows.size() << '\n';
    for (std::size_t rowIndex = 0; rowIndex < storage.rows.size(); ++rowIndex)
    {
        output << "row " << rowIndex;
        for (const KeyBinding& key : storage.rows[rowIndex])
        {
            output << ' ' << key.keyCode;
        }
        output << '\n';
    }
    return true;
}

void SaveCustomLayout(const CustomLayoutStorage& storage)
{
    SaveCustomLayoutToPath(storage, kCustomLayoutFilePath);
}

bool LoadCustomLayoutFromPath(CustomLayoutStorage& storage, const char* path)
{
    if (path == nullptr || path[0] == '\0')
    {
        return false;
    }

    std::ifstream input(path);
    if (!input.is_open())
    {
        return false;
    }

    std::vector<std::vector<KeyBinding>> loadedRows;
    bool includeMouse = true;

    std::string line;
    while (std::getline(input, line))
    {
        std::istringstream stream(line);
        std::string token;
        if (!(stream >> token))
        {
            continue;
        }

        if (token == "mouse")
        {
            int flag = 1;
            if (stream >> flag)
            {
                includeMouse = flag != 0;
            }
            continue;
        }

        if (token == "rows")
        {
            int rowCount = 0;
            if (stream >> rowCount && rowCount > 0)
            {
                loadedRows.resize(static_cast<std::size_t>(rowCount));
            }
            continue;
        }

        if (token != "row")
        {
            continue;
        }

        int rowIndex = -1;
        if (!(stream >> rowIndex) || rowIndex < 0)
        {
            continue;
        }

        const std::size_t rowIndexU = static_cast<std::size_t>(rowIndex);
        if (rowIndexU >= kMaxCustomRowCount)
        {
            continue;
        }
        if (loadedRows.size() <= rowIndexU)
        {
            loadedRows.resize(rowIndexU + 1U);
        }

        std::uint32_t keyCode = 0U;
        while (stream >> keyCode)
        {
            const KeyBinding* paletteKey = FindPaletteKeyByCode(keyCode);
            if (paletteKey != nullptr)
            {
                loadedRows[rowIndexU].push_back(*paletteKey);
            }
        }
    }

    if (loadedRows.empty())
    {
        return false;
    }

    storage.rows = std::move(loadedRows);
    storage.includeMouse = includeMouse;
    NormalizeCustomLayoutStorage(storage);
    return true;
}

void EnsureCustomStorageInitialized(CustomLayoutStorage& storage)
{
    if (storage.initialized)
    {
        return;
    }

    ResetCustomLayoutStorage(storage);
    LoadCustomLayoutFromPath(storage, kCustomLayoutFilePath);
    for (std::size_t i = 0; i < storage.paletteLabels.size(); ++i)
    {
        storage.paletteLabels[i] = kPaletteKeys[i].label;
    }
    storage.initialized = true;
}

void BuildCustomRowViews(CustomLayoutStorage& storage)
{
    storage.rowViews.clear();
    storage.rowViews.reserve(storage.rows.size() + (storage.includeMouse ? 1U : 0U));

    for (const std::vector<KeyBinding>& row : storage.rows)
    {
        KeyRow view{};
        view.keys = row.empty() ? nullptr : row.data();
        view.keyCount = row.size();
        storage.rowViews.push_back(view);
    }

    if (storage.includeMouse)
    {
        KeyRow mouseRow{};
        mouseRow.keys = kMouseRowKeys;
        mouseRow.keyCount = kMouseRowKeyCount;
        storage.rowViews.push_back(mouseRow);
    }
}

}

int GetLayoutPresetCount()
{
    return static_cast<int>(sizeof(kLayoutPresetLabels) / sizeof(kLayoutPresetLabels[0]));
}

const char* const* GetLayoutPresetLabels()
{
    return kLayoutPresetLabels;
}

KeyRowSet GetRowsForPreset(int presetIndex)
{
    if (presetIndex == kCustomPresetIndex)
    {
        CustomLayoutStorage& storage = GetCustomStorage();
        EnsureCustomStorageInitialized(storage);
        BuildCustomRowViews(storage);
        return KeyRowSet{ storage.rowViews.data(), storage.rowViews.size() };
    }

    if (presetIndex == 1)
    {
        return KeyRowSet{ kKeyboardMouseRows.data(), kKeyboardMouseRows.size() };
    }

    return KeyRowSet{ kKeyboardRows.data(), kKeyboardRows.size() };
}

void ApplyLayoutPresetDefaults(int presetIndex, float& layoutScale)
{
    switch (presetIndex)
    {
    case 0:
    case 1:
    case kCustomPresetIndex:
        layoutScale = 1.0f;
        break;
    default:
        break;
    }
}

bool IsCustomLayoutPreset(int presetIndex)
{
    return presetIndex == kCustomPresetIndex;
}

int GetCustomEditableRowCount()
{
    CustomLayoutStorage& storage = GetCustomStorage();
    EnsureCustomStorageInitialized(storage);
    return static_cast<int>(storage.rows.size());
}

const char* const* GetCustomEditableRowLabels()
{
    CustomLayoutStorage& storage = GetCustomStorage();
    EnsureCustomStorageInitialized(storage);
    return storage.rowLabelPtrs.empty() ? nullptr : storage.rowLabelPtrs.data();
}

int GetCustomPaletteKeyCount()
{
    return static_cast<int>(sizeof(kPaletteKeys) / sizeof(kPaletteKeys[0]));
}

const char* const* GetCustomPaletteKeyLabels()
{
    CustomLayoutStorage& storage = GetCustomStorage();
    EnsureCustomStorageInitialized(storage);
    return storage.paletteLabels.data();
}

bool AddCustomKeyByPaletteIndex(int paletteIndex, int rowIndex)
{
    CustomLayoutStorage& storage = GetCustomStorage();
    EnsureCustomStorageInitialized(storage);
    if (!IsEditableRowIndex(storage, rowIndex))
    {
        return false;
    }

    const int paletteCount = GetCustomPaletteKeyCount();
    if (paletteIndex < 0 || paletteIndex >= paletteCount)
    {
        return false;
    }

    std::vector<KeyBinding>& row = storage.rows[static_cast<std::size_t>(rowIndex)];
    if (row.size() >= kMaxCustomRowKeyCount)
    {
        return false;
    }

    row.push_back(kPaletteKeys[paletteIndex]);
    SaveCustomLayout(storage);
    return true;
}

bool AddCustomRow()
{
    CustomLayoutStorage& storage = GetCustomStorage();
    EnsureCustomStorageInitialized(storage);
    if (storage.rows.size() >= kMaxCustomRowCount)
    {
        return false;
    }

    storage.rows.emplace_back();
    NormalizeCustomLayoutStorage(storage);
    SaveCustomLayout(storage);
    return true;
}

bool RemoveCustomRow(int rowIndex)
{
    CustomLayoutStorage& storage = GetCustomStorage();
    EnsureCustomStorageInitialized(storage);
    if (!IsEditableRowIndex(storage, rowIndex))
    {
        return false;
    }
    if (storage.rows.size() <= kMinCustomRowCount)
    {
        return false;
    }

    storage.rows.erase(storage.rows.begin() + rowIndex);
    NormalizeCustomLayoutStorage(storage);
    SaveCustomLayout(storage);
    return true;
}

bool GetCustomIncludeMouse()
{
    CustomLayoutStorage& storage = GetCustomStorage();
    EnsureCustomStorageInitialized(storage);
    return storage.includeMouse;
}

void SetCustomIncludeMouse(bool includeMouse)
{
    CustomLayoutStorage& storage = GetCustomStorage();
    EnsureCustomStorageInitialized(storage);
    if (storage.includeMouse == includeMouse)
    {
        return;
    }

    storage.includeMouse = includeMouse;
    SaveCustomLayout(storage);
}

bool ExportCustomLayout(const char* path)
{
    CustomLayoutStorage& storage = GetCustomStorage();
    EnsureCustomStorageInitialized(storage);
    const char* targetPath = (path != nullptr && path[0] != '\0') ? path : kCustomLayoutExchangePath;
    return SaveCustomLayoutToPath(storage, targetPath);
}

bool ImportCustomLayout(const char* path)
{
    CustomLayoutStorage& storage = GetCustomStorage();
    EnsureCustomStorageInitialized(storage);
    const char* sourcePath = (path != nullptr && path[0] != '\0') ? path : kCustomLayoutExchangePath;
    if (!LoadCustomLayoutFromPath(storage, sourcePath))
    {
        return false;
    }

    SaveCustomLayout(storage);
    return true;
}

void ApplyCustomLayoutEditCommand(const CustomLayoutEditCommand& command)
{
    CustomLayoutStorage& storage = GetCustomStorage();
    EnsureCustomStorageInitialized(storage);

    if (command.removeRequested)
    {
        if (!IsEditableRowIndex(storage, command.removeRow))
        {
            return;
        }

        std::vector<KeyBinding>& row = storage.rows[static_cast<std::size_t>(command.removeRow)];
        if (command.removeIndex < 0 || static_cast<std::size_t>(command.removeIndex) >= row.size())
        {
            return;
        }

        row.erase(row.begin() + command.removeIndex);
        SaveCustomLayout(storage);
        return;
    }

    if (!command.moveRequested)
    {
        return;
    }

    if (!IsEditableRowIndex(storage, command.fromRow) || !IsEditableRowIndex(storage, command.toRow))
    {
        return;
    }

    std::vector<KeyBinding>& fromRow = storage.rows[static_cast<std::size_t>(command.fromRow)];
    std::vector<KeyBinding>& toRow = storage.rows[static_cast<std::size_t>(command.toRow)];
    if (command.fromIndex < 0 || static_cast<std::size_t>(command.fromIndex) >= fromRow.size())
    {
        return;
    }

    const KeyBinding movedKey = fromRow[command.fromIndex];
    fromRow.erase(fromRow.begin() + command.fromIndex);

    if (&fromRow != &toRow && toRow.size() >= kMaxCustomRowKeyCount)
    {
        const int safeRestoreIndex = (std::clamp)(command.fromIndex, 0, static_cast<int>(fromRow.size()));
        fromRow.insert(fromRow.begin() + safeRestoreIndex, movedKey);
        return;
    }

    int clampedInsertIndex = command.toIndex;
    if (clampedInsertIndex < 0)
    {
        clampedInsertIndex = 0;
    }
    if (static_cast<std::size_t>(clampedInsertIndex) > toRow.size())
    {
        clampedInsertIndex = static_cast<int>(toRow.size());
    }

    // 同行向后移动时，先删除再插入会导致目标索引偏移，这里做一次修正。
    if (command.fromRow == command.toRow && clampedInsertIndex > command.fromIndex)
    {
        --clampedInsertIndex;
    }
    if (clampedInsertIndex < 0)
    {
        clampedInsertIndex = 0;
    }

    toRow.insert(toRow.begin() + clampedInsertIndex, movedKey);
    SaveCustomLayout(storage);
}

void ResetCustomLayoutPreset()
{
    CustomLayoutStorage& storage = GetCustomStorage();
    EnsureCustomStorageInitialized(storage);
    ResetCustomLayoutStorage(storage);
    SaveCustomLayout(storage);
}
} // namespace keyviz
