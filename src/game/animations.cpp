#include "game/animations.h"

#include <array>

namespace {

// Flats
constexpr std::array<std::string_view, 3> nukage{"NUKAGE1", "NUKAGE2", "NUKAGE3"};
constexpr std::array<std::string_view, 4> fwater{"FWATER1", "FWATER2", "FWATER3", "FWATER4"};
constexpr std::array<std::string_view, 4> swater{"SWATER1", "SWATER2", "SWATER3", "SWATER4"};
constexpr std::array<std::string_view, 4> lava{"LAVA1", "LAVA2", "LAVA3", "LAVA4"};
constexpr std::array<std::string_view, 3> blood{"BLOOD1", "BLOOD2", "BLOOD3"};
constexpr std::array<std::string_view, 4> rrock{"RROCK05", "RROCK06", "RROCK07", "RROCK08"};
constexpr std::array<std::string_view, 4> slimeA{"SLIME01", "SLIME02", "SLIME03", "SLIME04"};
constexpr std::array<std::string_view, 4> slimeB{"SLIME05", "SLIME06", "SLIME07", "SLIME08"};
constexpr std::array<std::string_view, 4> slimeC{"SLIME09", "SLIME10", "SLIME11", "SLIME12"};

// Wall textures
constexpr std::array<std::string_view, 4> blodgr{"BLODGR1", "BLODGR2", "BLODGR3", "BLODGR4"};
constexpr std::array<std::string_view, 3> sladrip{"SLADRIP1", "SLADRIP2", "SLADRIP3"};
constexpr std::array<std::string_view, 4> blodrip{"BLODRIP1", "BLODRIP2", "BLODRIP3", "BLODRIP4"};
constexpr std::array<std::string_view, 3> firewall{"FIREWALA", "FIREWALB", "FIREWALL"};
constexpr std::array<std::string_view, 3> gstfont{"GSTFONT1", "GSTFONT2", "GSTFONT3"};
constexpr std::array<std::string_view, 3> firelava{"FIRELAVA", "FIRELAV2", "FIRELAV3"};
constexpr std::array<std::string_view, 3> firemag{"FIREMAG1", "FIREMAG2", "FIREMAG3"};
constexpr std::array<std::string_view, 2> fireblu{"FIREBLU1", "FIREBLU2"};
constexpr std::array<std::string_view, 3> rockred{"ROCKRED1", "ROCKRED2", "ROCKRED3"};
constexpr std::array<std::string_view, 4> bfall{"BFALL1", "BFALL2", "BFALL3", "BFALL4"};
constexpr std::array<std::string_view, 4> sfall{"SFALL1", "SFALL2", "SFALL3", "SFALL4"};
constexpr std::array<std::string_view, 4> wfall{"WFALL1", "WFALL2", "WFALL3", "WFALL4"};
constexpr std::array<std::string_view, 4> dbrain{"DBRAIN1", "DBRAIN2", "DBRAIN3", "DBRAIN4"};

constexpr std::array<AnimationDef, 22> definitions{{
  {false, nukage}, {false, fwater}, {false, swater}, {false, lava}, {false, blood},
  {false, rrock}, {false, slimeA}, {false, slimeB}, {false, slimeC},
  {true, blodgr}, {true, sladrip}, {true, blodrip}, {true, firewall}, {true, gstfont},
  {true, firelava}, {true, firemag}, {true, fireblu}, {true, rockred},
  {true, bfall}, {true, sfall}, {true, wfall}, {true, dbrain},
}};

} // namespace

std::span<const AnimationDef> animationDefs() {
  return definitions;
}
