#ifndef ENEMY_H
#define ENEMY_H

#include "vector3.h"
#include "character.h"
#include "record_data.h"

class Enemy : public Character
{
public:
    struct Control
    {
        uint8 script_active;
        uint8 hierarchy_ready;
        uint8 gravity_enabled;
        uint8 damage_phase;
        uint8 activation_count;
        bool linked_recs_activated;
        uint8 ground_period;
        uint8 ground_tick;
        uint16 prev_activation_flags;
        uint16 variant;
        uint16 height_limit;
        uint32 flags;
        uint16 script_delay;

        Control() : script_active(0), hierarchy_ready(0), gravity_enabled(0), damage_phase(0), activation_count(0), linked_recs_activated(false), ground_period(0), ground_tick(0), prev_activation_flags(0), variant(0), height_limit(32), flags(0), script_delay(0)
        {
        }
    } control;

protected:
    RecordCursor script_cursor;

public:
    struct AI
    {
        // Bytecode parameters and work slots have class-specific meanings
        uint16 parameters[10];
        uint16 work[6];
        uint16 state;
        uint16 phase;
        // Slot nine of the bytecode register bank has no named behavior consumer
        uint16 script_register9;
        // Model replacement key or Swat voice mask according to behavior kind
        uint32 script_data;

        AI() : state(0), phase(0), script_register9(0), script_data(0)
        {
            uint32 idx;
            for (idx = 0; idx < 10; ++idx)
                parameters[idx] = 0;
            for (idx = 0; idx < 6; ++idx)
                work[idx] = 0;
        }

        void SetStatePhase(uint32 value)
        {
            state = value;
            phase = value >> 16;
        }
    } ai;

    sint16 ground_offset;
    sint16 sight_offset;
    Vector3 dst;

private:
    RecordCursor script_labels[8];
    uint16 *behavior;
    uint8 behavior_rows;
    uint8 behavior_cols;

protected:
    Enemy();

public:
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Finish();

protected:
    uint32 ScaleDamageForDifficulty(uint32 damage);

public:
    uint32 ActivateLinkedRecs();
    uint32 IntegrateMotion();

protected:
    bool ChoosePos(Vector3 *output, const Vector3 *target, uint32 minimum, uint32 maximum, sint32 height_limit, sint16 spread, bool ground, bool check_col);
    void AllocateBehaviorTable(uint8 rows, uint8 columns);
    void FreeBehaviorTable();
    void SetBehaviorVal(uint8 row, uint8 column, uint16 val);
    uint16 GetBehaviorVal(uint8 row, uint8 column) const;

private:
    uint16 SkipConditionalBlock();

protected:
    uint32 ExecuteScript(const RecordCursor &script);

public:
    virtual uint32 ExecuteCommand(uint16 opcode);
    uint16 ReadScriptVal();
    uint32 ReadParameters(uint16 command);
    virtual uint32 ResolveParameter(uint16 opcode);
    virtual void NoOp();
    static void ResetPopulation();
    virtual ~Enemy();
    virtual void SetParameters(uint16 command);

protected:
    RecordCursor GetScriptLabel(uint32 idx) const;
    uint16 ReadCommandWord();

private:
    void SetScriptLabel(uint32 idx, const RecordCursor &script);
    uint16 ReadCommandVal();
    sint32 ReadSignedCommandVal();
    uint16 ReadRegister(uint32 idx) const;
    void WriteRegister(uint32 idx, uint16 val);
    void WriteAIParameter(uint32 idx, uint16 val);
    uint16 GetBehaviorIdx(uint8 row, uint8 column) const;
};

#endif
