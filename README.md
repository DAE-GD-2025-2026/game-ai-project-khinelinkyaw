# Game AI Project

## Extra Assignment
- Get to the closest node if the final destination could not be reached (Fallback Path)

## A-star Path Finding Algorithm
- Handles special case when a node is visited for a second time
- If no full path to the goal is found, the nearest node is chosen as the destination

## NavMesh Path Finding
- Based on A-star path finding algorithm
- Find paths using the nodes on the midpoint of each edge with two triangles neighboring it
- Path is optimized using SSFA (Stupid Simple Funnel Algorithm)
