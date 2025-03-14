var canvas;
var canvasWidth = 1000;
var canvasHeight = 608;
var ctx;
var canvasData;
var background;
var id_counter = 0;
let cursor_x = 0;
let cursor_y = 0;

// Initial Units
const pink = "#ff00ff";
const green = "#00ff00";
const system = "#00ffff";

const unit_costs = 1000;
const kill_reward = 1700;
const base_reward = 100000;

// Initial units marking pink-winning battlefield
const initial_pink = [[168, 346], [511, 207], [841, 259], [50, 177]];
// Initial units marking green-winning battlefield
const initial_green = [[801, 391], [582, 449], [546, 306], [877, 154]];

// pink and green units. Each unit has a position (x, y) and a strength. The 
// strength determines the how big a unit is rendered (1-4 -> 1, 5-9 -> 2, ...)
// If a units strength is 0 it is removed from the battlefield.
// If a units strength is -1 it is a unmoving unit 
var pinks = {
  chance: 100,
  units: {},
  num_units: 0,
  dollars: 0,
  // Initial single-units, marking pink territory
  bases: [[139, 201], [207, 205], [135, 236], [197, 245], [439, 204], [437, 245], 
    [475, 217], [482, 170], [504, 157], [828, 457], [873, 463], [868, 491], [499, 291], 
    [550, 286], [507, 471], [543, 453]],
  bases_destroyed: 0,
}
var greens = {
  chance: 100,
  units: {},
  num_units: 0,
  dollars: 0,
  // Initial single-units, marking green territory
  bases: [[233, 418], [298, 424], [262, 471], [246, 517], [446, 295], [417, 328], 
    [498, 343], [503, 386], [617, 179], [698, 163], [782, 178], [781, 259], [765, 286], 
    [719, 255], [690, 314]],
  bases_destroyed: 0,
}

window.onload = function() {
  'use strict'; 
  // Setup canvas
  canvas = document.getElementById("battlefield");
  canvas.width = canvasWidth;
  canvas.height = canvasHeight;
  ctx = canvas.getContext("2d");
  canvasData = ctx.getImageData(0, 0, canvasWidth, canvasHeight);

  // Load background image to canvas
  const image = new Image();
  image.src = 'images/neon-world-map.jpg';
  image.onload = function() {
    ctx.drawImage(image, 0, 0);
  };
  background = image;

  // Get canvas position on mouseclick
  canvas.addEventListener('mousedown', function(e) {
      getCursorPosition(canvas, e)
  });
}

function UpdateChance(chance, pink) {
  if (pink) {
    pinks.chance = chance;
    console.log("Pink chance updated: ", pinks.chance);
  }
  else {
    greens.chance = chance;
    console.log("Green chance updated: ", greens.chance);
  }
}

// Unit Movement

/** 
 * Generates green and pink units from initial lists
 */
function Setup() {
  fetch("/api/bf/map")
    .then(response => response.json())
    .then(json => {
      console.log(json);
      if (json.victory !== "") {
        ctx.fillStyle = system;
        ctx.font = "30px Arial";
        if (json.victory === "pink") {
          ctx.fillText("PINK VICTORY", 500, 200);
        } else if (json.victory === "green") {
          ctx.fillText("GREEN VICTORY", 460, 300);
        }
      } else {
        print_fields(json.pink_army, json.green_army);
      }
    });
}

function CreateBases(player) {
  for (const [x, y] of player.bases) {
    player.units[CreateId()] = {pos: [x, y], strength: 1, base: true};
  }
}

function CreateUnitsAroundCenter(x, y, units, fak) {
  const dist = 20;
  // Get how many units are near given position and create units
  var initial_units_at_pos = ran(10 + fak*2, 20 + fak*2); 
  for (var i=0; i<initial_units_at_pos; i++) {
    // Get strength of unit
    var unit_strength = ran(3 + fak*1, 7 + fak*1);
    units[CreateId()] = {
      pos: [ran_close(x, dist), ran_close(y, dist)], 
      strength: unit_strength
    }
  }
}

async function DoPhases() {
  const delay = ms => new Promise(res => setTimeout(res, ms));
  fetch("/api/bf/unpause", { method: "POST"})
    .then(response => console.log("(unpause) Response ok:", response.ok));
  while (true) {
    await delay(250);
    fetch("/api/bf/map")
      .then(response => response.json())
      .then(json => {
        console.log(json);
        if (json.victory !== "") {
          ctx.fillStyle = system;
          ctx.font = "30px Arial";
          if (json.victory === "pink") {
            ctx.fillText("PINK VICTORY", 500, 200);
          } else if (json.victory === "green") {
            ctx.fillText("GREEN VICTORY", 460, 300);
          }
        } else {
          print_fields(json.pink_army, json.green_army);
        }
      });
  }
  
  // let counter = 0;
  // while (CollectBases(pinks.units).length > 0 && CollectBases(greens.units).length > 0) {
  //   await delay(250);
  //   let num_phases = document.getElementById("num_phases").value;
  //   // Do unit-movement and attacks
  //   for (let i=0; i<num_phases; i++) {
  //     OnePhase(pinks, greens);
  //     if (CollectBases(pinks.units).length <= 0 || CollectBases(greens.units).length <= 0)
  //       break;
  //     OnePhase(greens, pinks);
  //     if (CollectBases(pinks.units).length <= 0 || CollectBases(greens.units).length <= 0)
  //       break;
  //   }
  //   // Create new units
  //   if (counter%4 === 0) {
  //     SendNewUnits(pinks);
  //     SendNewUnits(greens);
  //   }
  //   print_fields();
  //   counter++;
  // }
  // print_fields();
  // ctx.fillStyle = system;
  // ctx.font = "30px Arial";
  // if (CollectBases(pinks.units).length > 0) {
  //   ctx.fillText("PINK VICTORY", 500, 200);
  // } else {
  //   ctx.fillText("GREEN VICTORY", 460, 300);
  // }
}

function SendNewUnits(player) {
  let dist = 5;
  let money_to_spend = unit_costs + player.dollars*0.1;
  while (money_to_spend >= unit_costs) {
    // Collect bases (units with initial strength of -1
    let bases = CollectBases(player.units);
    // For each base create one unit
    for (let i=0; i<bases.length; i++) {
      const [x, y] = bases[ran(0, bases.length-1)];
      let possible_strength = 1 + Math.floor(money_to_spend/(unit_costs*5));
      if (pinks.dollars >= unit_costs*possible_strength) {
        player.units[CreateId()] = {
          pos: [ran_close(x, dist), ran_close(y, dist)],
          strength: possible_strength 
        }
        player.dollars -= unit_costs * possible_strength;
      }
      money_to_spend -= unit_costs * possible_strength;
      if (money_to_spend < unit_costs) 
        break;
    }
  }
}

function OnePhase(atks, defs) {
  const atks_cpy = JSON.parse(JSON.stringify(atks.units));
  for (const [id, unit] of Object.entries(atks_cpy)) {
    // Skip unmoving units
    if (unit.base) 
      continue;
    // Get Position and find closet enemy
    const [x, y] = unit.pos;
    const [enemy_id, x2, y2] = FindClosestEnemy(x, y, defs.units);
    // If no position could be found, continue
    if (enemy_id === -1)
      continue;
    // Either attack enemy or move towards enemy depending on distance
    dist = getDistance(x, y, x2, y2);
    if (dist > 3) {
      MoveUnitTowardsEnemy(id, x, y, x2, y2, (dist>50) ? 3 : 1, atks.units);
    } else {
      MakeAttack(id, unit.strength, enemy_id, atks, defs);
    }
  }
}

function FindClosestEnemy(x, y, enemies) {
  let min_dist = 2000;
  let current_pos = [-1, -1, -1];
  for (let [id, unit] of Object.entries(enemies)) {
    const [x2, y2] = unit.pos;
    let dist = getDistance(x, y, x2, y2);
    if (dist < min_dist) {
      min_dist = dist; 
      current_pos = [id, x2, y2];
    }
  }
  return current_pos;
}

function CalculateNewPosition(x1, y1, x2, y2, stepSize) {
  let dx = x2 - x1;
  let dy = y2 - y1;
  let distance = Math.sqrt(dx * dx + dy * dy); // Euclidean distance

  // If already at the target or very close, return the target position
  if (distance < stepSize) 
    return [x2, y2];

  // Normalize direction
  let nx = dx / distance;
  let ny = dy / distance;

  // Move in the direction by stepSize
  return [Math.round(x1 + nx * stepSize), Math.round(y1 + ny * stepSize)];
}

function MoveUnitTowardsEnemy(id, x1, y1, x2, y2, stepSize, units) {
  const new_pos = CalculateNewPosition(x1, y1, x2, y2, stepSize);
  units[id].pos = new_pos;
}

function MakeAttack(id_atk, strength, id_def, atks, defs) {
  let pos = atks.units[id_atk].pos;
  let strength_def = defs.units[id_def].strength;
  let dominance = HasDominance(pos, atks, defs, 20);
  console.log("HasDominance?", dominance);
  if (ran(0, atks.chance - 5*dominance) === 0) {
    DoSuccessfulAttack(strength, id_def, strength_def, atks, defs);
  } else if (ran(0, defs.chance - 5*(!dominance)) === 0) {
    DoSuccessfulAttack(strength_def, id_atk, strength, defs, atks);
  }
}

function DoSuccessfulAttack(strength_atk, id_def, strength_def, atks, defs) {
  let new_strength = strength_def - (1+strength_atk*0.1);
  // If strength reaches zero, remove unit and grant enemy dollars
  if (new_strength <= 0) {
    if (defs.units[id_def].base) {
      atks.dollars += base_reward;
      defs.bases_destroyed++;
      console.log("base destroyed", defs.cursor_x, 35 + defs.cursor_y);
    }
    atks.dollars += kill_reward;
    delete defs.units[id_def];
  } else {
    defs.units[id_def].strength = new_strength;
  }
}

function CollectBases(units) {
  var bases = [];
  for (const [_, unit] of Object.entries(units)) {
    if (unit.base) 
      bases.push(unit.pos);
  }
  return bases;
}

// Draw methods 

function print_fields(pink_army, green_army) {
  ctx.drawImage(background, 0, 0, canvas.width, canvas.height);
  // Print pink units and add up total of units (strength of all units)
  for (const unit of pink_army.units) {
    const [x, y] = unit.pos;
    draw_pink(x, y, CalcSizeFromStength(unit.strength));
  }
  // Print green units and add up total of units (strength of all units)
  for (const unit of green_army.units) {
    const [x, y] = unit.pos;
    draw_green(x, y, CalcSizeFromStength(unit.strength));
  }
  // // Calculate final unit
  // pinks.num_units = Math.floor(pinks.num_units);
  // greens.num_units = Math.floor(greens.num_units);
  // // Initially set dollars
  // if (pinks.dollars === 0 && greens.dollars === 0) {
  //   pinks.dollars = pinks.num_units*unit_costs;
  //   greens.dollars = greens.num_units*unit_costs;
  // }

  ctx.fillStyle = system; 
  ctx.font = "20px Arial";
  ctx.fillText("BATTLEFIELD", 10, 30);
  ctx.font = "14px Arial";
  ctx.fillText("pink (units: " + pink_army.num_units + ", " + pink_army.dollars + "$) " 
    + "green: (units: " + green_army.num_units + ", " + green_army.dollars + "$)", 600, 30);
  for (let i=0; i<pink_army.bases_destroyed; i++) {
    ctx.fillText("base destroyed...", 600, 42 + i*10);
  }
   for (let i=0; i<green_army.bases_destroyed; i++) {
    ctx.fillText("base destroyed...", 792, 42 + i*10);
  }
}

function draw_dot(x, y, size, color) {
  ctx.beginPath();
  ctx.fillStyle = color; // Change color as needed
  ctx.arc(x, y, size, 0, 2 * Math.PI, true);
  ctx.fill();
}

function draw_pink(x, y, size) {
  draw_dot(x, y, size, pink);
}
function draw_green(x, y, size) {
  draw_dot(x, y, size, green);
}

// Helpers

function countPositionsWithinRadius(target, list, radius) {
  const radiusSquared = radius * radius; // Use squared distance for efficiency

  return list.filter(([x, y]) => {
      const dx = x - target[0];
      const dy = y - target[1];
      return dx * dx + dy * dy <= radiusSquared; // Avoid square root for efficiency
  }).length;
}

function HasDominance(target, atks, defs, radius) {
  const dictToPositionList = (dict) => Object.values(dict).map(unit => unit.pos);
  const count1 = countPositionsWithinRadius(target, dictToPositionList(atks.units), radius);
  const count2 = countPositionsWithinRadius(target, dictToPositionList(defs.units), radius);

  return count1 > count2;
}

/** 
 * Returns random number between start and end
 */
function ran(start, end) {
  return Math.floor(Math.random() * (end-start +1)) + start;
}

/**
 * Gets a number within a given distance from the inital number.
 */
function ran_close(x, dist) {
  return x + ((ran(0, 1) === 1) ? -1 : 1) * ran(0, dist);
}

/**
 * Calculates size to render from unit_strength
 */
function CalcSizeFromStength(unit_strength) {
  if (unit_strength === -1)
    return 1;
  return Math.floor(unit_strength/5) + 1;
}

function getDistance(x1, y1, x2, y2) {
  return Math.sqrt(Math.pow(x2 - x1, 2) + Math.pow(y2 - y1, 2));
}

function CreateId() {
  return pad(id_counter++) + ":" + [ran(0,9), ran(0,9), ran(0,9), ran(0,9), ran(0,9)].join("");
}
function pad(num) {
  num = num.toString();
  while (num.length < 5) num = "0" + num;
  return num;
}

function CreateBase(pink) {
  if (pink && pinks.dollars >= base_reward) {
    pinks.units[CreateId()] = {pos: [cursor_x, cursor_y], strength: 1, base: true};
  } else if (green && greens.dollars >= base_reward) {
    greens.units[CreateId()] = {pos: [cursor_x, cursor_y], strength: 1, base: true};
  } else {
    alert("Not enough money");
  }
}

function getCursorPosition(canvas, event) {
  const rect = canvas.getBoundingClientRect()
  cursor_x = event.clientX - rect.left
  cursor_y = event.clientY - rect.top
}

