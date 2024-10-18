Autonomous city sim

1) How do you determine waypoints ?
--> To place the houses and shops I use the same system as for the roads, that is to say a grid, but to define the destination I put the coordinates and the citizen will go alone to the destination via a pathfing system.

3) What is the best way to allow NPC to go from and to their workplace without collisions if the map is static (not changed at runtime) ?
--> We can assign a space to a citizen and wait until the space is empty so that the next citizen can go to it, so traffic will be fluid.

4) What if they change workplace every day ?
--> You will just have to enter the coordinates of the new workplace like in real life, you have to know the address before going there

5) What happens when there is a crossroad ?
--> when there is a crossroad, it's the same principle as said previously, each space has a citizen and we wait until the space is empty to go there,
   except that if the desired direction is already taken by a citizen then, Astar will define a new route to reach the destination

6) How can you make your roads directional ?
--> For the roads to be usable only in one direction it would be necessary to make a road with a different number in the grid which could be usable only for example if the movement of the citizen is with an x+ so that it is only in this direction.

7) What if we create a new road at runtime ?
--> The route should be taken into account with AStar if it is added correctly in the grid.

------------------------------------------------------------------------------------------------
detail your choices and give an overview of the framerate obtained with 1, 10, 100 and 500 agents or more if you can ! 
Give a simple explanation of the improvements you can think of to improve your system and solve the questions from the chosen problem.

--> 1 - 500 agents : 60 fps / we start to have some fps loss at 1200 agents
For FPS, they will start to drop the more we increase the number of citizens because each citizen will look for their destination and calculate the most optimal path to get there.

the main problem with my code is the citizen movement system, which means that they are one in a box so we can easily find ourselves stuck in certain situations like a crossroads or if there are a lot of citizens on the map this which would cause a general blockage, 
the solution would be not to limit the boxes to one citizen but two side by side as for the roads in both directions that we see everywhere outside
