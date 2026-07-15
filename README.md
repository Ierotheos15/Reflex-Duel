# Reflex Duel

A two-player reaction time game on Arduino UNO. Both players get a button — wait for **GO!** to flash on the LCD and hit yours first. Press too early and the other player gets the point. First to 3 wins.

The Arduino handles the countdown, randomized delay before GO, false start detection, reaction time display, LED feedback, buzzer sounds, and score tracking.

Built for Hack Club Breadboard YSWS.

🔗 [Open in Simulator](https://breadboard.hackclub.com/editor/164)

## Wiring

![Wiring Diagram](wiring_diagram.png)

| Component | Arduino Pin |
|---|---|
| Player 1 Button | D2 |
| Player 2 Button | D3 |
| Player 1 Blue LED (win) | D4 |
| Player 1 Red LED (lose) | D5 |
| Player 2 Yellow LED (win) | D6 |
| Player 2 Red LED (lose) | D7 |
| Passive Buzzer | D8 |
| LCD SDA | A4 |
| LCD SCL | A5 |

## Bill of Materials

| Component | Qty |
|---|---|
| Arduino UNO R3 | 1 |
| Breadboard | 1 |
| Tact Switch 12x12 | 2 |
| LED Red F5 | 2 |
| LED Blue F5 | 1 |
| LED Yellow F5 | 1 |
| Resistor 220Ω | 4 |
| Passive Buzzer | 1 |
| LCD1602 | 1 |
| LCD1602 I2C Adapter | 1 |

## Libraries

- **LiquidCrystal_I2C** by Frank de Brabander — [GitHub](https://github.com/johnrickman/LiquidCrystal_I2C)

## Files

- `firmware/reflex_duel.ino` — Arduino sketch
- `wiring/wiring_diagram.png` — wiring diagram
- `Libraries/` — LiquidCrystal_I2C