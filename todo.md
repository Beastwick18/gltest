gltest
- struct containing norm direction (or just enum for which face), x y z position, texture id, and light value (width and height too?)
- these structs will be generated as intermittent objects for greedy meshing
- texture id is int generated for each 2d texture.
- for greedy meshing, compare these structs by moving in the x, y, z directions to find adjacent block faces with same x, y, or z, same texture id, same normal (block face must be on the same plane), and same light value
- Psuedo mesh object generated from rebuild mesh
- This psuedo mesh will have x, y, z, w, h
- 3 planes in which block faces exist:
- vertical                    |
- perpindicular to vertical   ■
- horizontal                  _
- Actually, must only combine block faces with same normal

- video on impl but in roblox:
- https://www.youtube.com/watch?v=L6P86i5O9iU

<!-- - Lighting -->
<!-- - Have global buffer in renderer/chunk for light bleed over chunk borders -->
<!-- - 4 buffers for each direction (left right forward back) -->
<!-- - buffer is size 16*256, no z depth. -->
<!-- - Have function in other chunk recieve a buffer, as well as which side the array is coming from -->
<!-- - This function will generate fake block lights -->
<!-- - When light attempts to go past chunk edge -->
<!-- - Buffers memset to 0 once done -->


- At this point i should probably rewrite the whole thing

- Switch to vector of chunks for storing all chunks, instead of 2d array of chunks. May make infinite worlds much easier
- Update chunk async. In remesh function, create temporary copy of chunk to modify. Once done rebuilding, copy over new data.
- Probably do thread pooling, so no slowdown because of generating new threads everytime we need to rebuild a chunk

# Light alternative
- Light alternative
- When trying to go past chunk edge, try to call rec light function for adjacent chunk with appropriate values. (assuming adjacent chunk isnt NULL)
- do nothing if null
- keep track of which chunks were modified
<!-- ## Queue -->
<!-- - maybe have queue in renderer for chunks to be redrawn. -->
<!-- - queue would only allow unique chunks, so no rerendering of a chunk twice -->
<!-- - Request -->
<!-- - another option would be to have each chunk have a bool attribute that is set when it wants to be rerendered -->
<!-- - a chunk mananger/the renderer would go through all chunks and rebuild them -->
<!-- - this way we could control how many chunks can be rebuilt per update -->
<!-- - We would set request_rebuild to true as soon as we bleed into that chunk -->
<!-- - This would allow for block light bleeding and sunlight bleeding -->

- Will likely have to remove max and min block optimization for lighting. Light bleed wont occur for areas above/below the max and min area
- One possible way to reduce performance impact of this change is square chunks of 32x32x32 (or any other appropriate value)
- Would mean we would have to have light bleed vertically and downwards alongside all horizontal bleeding
- would mean less blocks to loop over
- would mean more chunks to spread between and more chunk updates 
