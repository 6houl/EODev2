# UI Implementation Plan

**Date Started:** 2026-09-29  
**Goal:** Complete all remaining UI elements systematically with proper commits

---

## Implementation Strategy

### Approach
1. Pick low-hanging fruit first (Missing → Shell → Partial)
2. One feature per commit with natural commit messages
3. Test each feature before committing
4. Update UI_COMPLETION.md status after each completion
5. Close GitHub issues when fully complete

### Commit Message Style
- Focus on what was done, not how
- Use present tense ("Add session EXP tracking")
- Be concise but descriptive
- Example: "Implement party panel HP display and member list"

---

## Priority Queue (Easiest First)

### Phase 1: Quick Wins (1-2 days)

#### 1. Session EXP Display (Issue #35)
**Status:** Missing → Implemented  
**Effort:** Low  
**Files:** Map_Player.h/cpp, Map_UI_CharacterStats.cpp

**Implementation:**
- Add `sessionStartExp` to Map_Player
- Initialize on Welcome/Reply
- Calculate and display `exp - sessionStartExp`
- Add reset on map change or logout

**Commit:** "Add session EXP tracking to character stats panel"

---

#### 2. Help/Status Message Polish (Issue #41)
**Status:** Implemented → Complete  
**Effort:** Very Low  
**Files:** Just needs live testing

**Action:** Live test and mark complete  
**Commit:** "Verify help message display and lifetime"

---

#### 3. Status Clock Visual Check (Issue #38)
**Status:** Implemented → Complete  
**Effort:** Very Low  
**Files:** Just needs live testing

**Action:** Live test position and verify HH:MM:SS format  
**Commit:** "Verify status clock rendering at classic position"

---

### Phase 2: Shell → Partial (2-3 days)

#### 4. Party Panel HP Display (Issue #28)
**Status:** Shell → Partial  
**Effort:** Medium  
**Files:** Map_UI_Party.cpp/h

**Implementation:**
- Check if player has party state
- Display party member names
- Show HP bars for members
- Mark leader with icon
- Reference EndlessClient `PartyPanel.cs`

**Commit:** "Add party member display with HP bars and leader indicator"

---

#### 5. Passive Skills Content (Issue #46)
**Status:** Shell → Partial  
**Effort:** Low-Medium  
**Files:** Map_UI_PassiveSkills.cpp

**Implementation:**
- Display learned passive skills from character data
- Show skill icons and names
- Use same pattern as active skills
- Reference ECF for skill data

**Commit:** "Display passive skills from character data"

---

#### 6. News Panel Content (Issue none)
**Status:** Shell → Partial  
**Effort:** Low  
**Files:** Map_UI_News.cpp

**Implementation:**
- Connect to server news source if available
- Display scrollable news text
- Show "No news available" if no source
- Format with proper line breaks

**Commit:** "Connect news panel to server news data"

---

### Phase 3: Partial → Implemented (3-5 days)

#### 7. Chat Mode Completion (Issue #31)
**Status:** Partial → Implemented  
**Effort:** Medium  
**Files:** Map_UI_Talk.cpp/h

**Implementation:**
- Complete all chat modes (public, global, whisper, guild, party)
- Proper message routing per mode
- Color coding per mode
- Tab switching

**Commit:** "Complete chat mode switching and message routing"

---

#### 8. Whisper Tab System (Issue #32)
**Status:** Partial → Implemented  
**Effort:** Medium  
**Files:** Map_UI_Talk.cpp/h

**Implementation:**
- Track active whisper conversations
- Create/destroy tabs dynamically
- Store session target data
- Switch between conversations

**Commit:** "Implement whisper tab management with session tracking"

---

#### 9. Chat Presentation (Issue #33)
**Status:** Partial → Implemented  
**Effort:** Low-Medium  
**Files:** Map_UI_Talk.cpp/h

**Implementation:**
- Add lock state visual
- Highlight active tab
- Proper tab rendering
- Tab close buttons

**Commit:** "Add chat lock state and active tab visuals"

---

#### 10. Player Context Menu (Issue #34)
**Status:** Partial → Implemented  
**Effort:** Medium-High  
**Files:** Map_UI_SelectPlayer.cpp/h

**Implementation:**
- Complete all context menu actions
- Permission checks (range, admin level)
- Trade request
- Add friend
- Ignore player
- Attack (if appropriate)

**Commit:** "Complete player context menu with permission checks"

---

### Phase 4: Account/Login Polish (2-3 days)

#### 11. Account/Login Response Handling (Issue #128)
**Status:** Partial → Implemented  
**Effort:** Medium  
**Files:** Menu.cpp, Packet handlers

**Implementation:**
- Map all EOProtocol responses
- Clear user feedback for each
- Proper error messages
- Success confirmations

**Commit:** "Add comprehensive account and login response handling"

---

#### 12. Account Validation (Issue #131)
**Status:** Partial → Implemented  
**Effort:** Low-Medium  
**Files:** Menu.cpp

**Implementation:**
- Username length (4-16 chars)
- Password length (6+ chars)
- Valid characters only
- Real-time validation feedback

**Commit:** "Implement account creation validation rules"

---

#### 13. Character Creation Validation (Issue #132)
**Status:** Partial → Implemented  
**Effort:** Low-Medium  
**Files:** Menu.cpp

**Implementation:**
- Name length validation
- Name character validation
- Prevent duplicate names locally
- Server response handling

**Commit:** "Add character creation validation and error handling"

---

#### 14. Password Change Flow (Issue #60)
**Status:** Partial → Implemented  
**Effort:** Medium  
**Files:** Menu.cpp, Packet handlers

**Implementation:**
- Complete validation
- Old password verification
- New password confirmation
- Response handling

**Commit:** "Complete password change functionality"

---

### Phase 5: Complex UI (5-7 days)

#### 15. Paperdoll Completion (Issues #94-99)
**Status:** Partial → Implemented  
**Effort:** High  
**Files:** Multiple paperdoll files

**Implementation:**
- Remote equipment viewing
- Paired weapon/shield slots
- Complete graphic IDs
- Safe string handling
- All stat display

**Commit:** "Complete paperdoll with remote viewing and equipment display"

---

#### 16. Chest Interaction (Issues #101-102)
**Status:** Partial → Implemented  
**Effort:** Medium  
**Files:** Chest handler files

**Implementation:**
- Session isolation
- Range checking
- Update handling
- Close behavior
- Error states

**Commit:** "Complete chest interaction with session and range management"

---

#### 17. Shop/Craft System (Issues #103-105)
**Status:** Partial → Implemented  
**Effort:** High  
**Files:** Shop/craft UI files

**Implementation:**
- Welcome text display
- Variable recipe support
- All failure states
- Buy/sell/craft flows

**Commit:** "Implement complete shop and craft system"

---

#### 18. Quantity Dialogs (Issues #106-107)
**Status:** Partial → Implemented  
**Effort:** Low-Medium  
**Files:** Quantity dialog files

**Implementation:**
- Safe numeric parsing
- Input validation
- Pending state handling
- Cancel support

**Commit:** "Add quantity dialog with input validation"

---

### Phase 6: Configuration (2-3 days)

#### 19. Settings Panel Completion (Issue #25)
**Status:** Partial → Implemented  
**Effort:** Medium  
**Files:** Map_UI_GameSettings.cpp

**Implementation:**
- All configuration options
- Audio settings (when audio implemented)
- Localization options
- Save/load all settings

**Commit:** "Complete settings panel with all configuration options"

---

#### 20. Panel State Persistence (Issue #44)
**Status:** Missing → Implemented  
**Effort:** Medium  
**Files:** Map_UI.cpp, Config files

**Implementation:**
- Remember which panel was open
- Save movable dialog positions
- Restore on login
- Config file storage

**Commit:** "Add panel state persistence across sessions"

---

### Phase 7: Future Work (Blocked or Low Priority)

These require protocol work or are lower priority:

- Issue #36: Quest status (requires quest protocol)
- Issue #37: Friends/ignore lists (needs persistence)
- Issue #43: Loading state indicators
- Issue #45: Macros (needs design decision)
- Issue #47-59: Service dialogs (require protocol)
- Issue #140: Audio system
- Issue #141: Localization
- Issue #142: Complete persistence

---

## Testing Checklist Per Feature

Before each commit:

- [ ] Code compiles (Release|Win32, 0 errors)
- [ ] No new warnings introduced
- [ ] Feature works as expected
- [ ] No crashes or hangs
- [ ] Thread-safe if touching shared state
- [ ] Matches EndlessClient behavior

After commit:

- [ ] Update UI_COMPLETION.md status
- [ ] Update GitHub issue if applicable
- [ ] Note any remaining work
- [ ] Tag with issue number in commit

---

## Git Workflow

```bash
# Before starting work
git status
git pull origin main

# Make changes
# Test changes

# Stage specific files only
git add [files changed]

# Commit with descriptive message
git commit -m "Add session EXP tracking to character stats panel

Tracks experience gained since login and displays in stats panel.
Session baseline is set on Welcome/Reply and persists until logout.

Closes #35"

# Push to GitHub
git push origin main
```

---

## Current Status

**Started:** 2026-09-29  
**Phase:** Phase 3 - Partial → Implemented  
**Next Action:** Continue with context menu actions

**Progress Tracking:**
- Phase 1: 1/3 complete ✓
- Phase 2: 0/3 complete
- Phase 3: 2/4 complete ✓
- Phase 4: 0/4 complete
- Phase 5: 0/4 complete
- Phase 6: 0/2 complete

**Total:** 3/20 items complete

**Completed:**
- ✓ Item 1: Session EXP Display (Commit: abae8fd)
- ✓ Item 8 (Partial): Whisper Tab Tracking (Commit: 2637d1e)
- ✓ Item 10 (Partial): Player Context Menu Whisper (Commit: 8736973)

---

## Notes

- Keep commits focused on one feature
- Test thoroughly before pushing
- Update documentation after each phase
- Close issues only when feature is fully complete and tested
- Maintain consistent code style with existing codebase
