Milestone
=========

A simple arcade game, originally made in 36 hours using SDL, OpenGL, and using
some utility code taken from my [Azimuth](https://github.com/mdsteele/azimuth)
project.  I may well work on it a bit more in the future, but you can see where
it was at the 36-hour mark by checking out the
[hour36](https://github.com/mdsteele/milestone/releases/tag/hour36) tag.

The game plays a bit like [Crystal
Quest](http://en.wikipedia.org/wiki/Crystal_Quest) -- there are targets to
collect, and enemies to get in your way -- except that in Milestone you are
invincible and can only lose by running out of time.  Each wave lasts a fixed
amount of time, and on each wave you can collect not only the targets for that
wave (you lose a life if you don't get them all before the timer expires), but
also the targets for the next wave, which allows you to get a head-start but
also distracts you from the current wave.  If you collect _both_ wave's targets
before the first wave timer expires, you skip the next wave entirely and earn
an extra life.  Enemies can't kill you, but they can knock you around, hide
targets from you, or otherwise make your job harder.  Fortunately, you can stun
or kill enemies by ramming into them or shooting them -- but again, this
distracts you from the task of collecting all the targets within the time
limit.

This codebase is licensed under the GNU GPL, version 3.  This codebase is free
software: you can redistribute it and/or modify it under the terms of the GNU
General Public License as published by the Free Software Foundation, either
version 3 of the License, or (at your option) any later version.

Milestone is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

The complete license can be found in the LICENSE file.
