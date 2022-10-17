# New idea for lighting accurately
- calcSun/Light return true if any light changed, false otherwise
- when recalculating bleed lighting, scan edge of adj chunks, then do recursive lighting but allow moving between chunks (combine the two)
    - ONLY scan light array for new bleed, not lightBleed. lightBleed should be empty
    - curr chunk lightbleed array will be used for recalc bleed lighting
        - scan edge, calcbleedlight/sunlight (stored in lightbleed)
- when calling calclighting for other chunk, if it returns true mark other chunk as dirty
- bleed lighting held in seperate array from normal lighting
- light value of current block is just max between light/lightBleed
- this way we only have to clear lightBleed and recalc it instead of all lights
- dont have to worry about normal light bleed, as we will scan over adj chunks
- this way we also dont have to update all surrounding chunks every time we update chunk
    - only dirty will update, and much smaller update
- when rebuilding chunk in multithread, do not recalc bleed
    - otherwise, many threads could be in eachothers crit region, causing deadlock
- Clear adj chunk lightbleed array BEFORE recalculating current chunk
    - lightbleed that might wrap around will be recalculated when current chunk bleeds into adj ones
- Only 1 chunk rebuild per update
- dont have to do full rebuild on dirty chunks. only rebuildmesh
- fix occurences of light\[y\]\[x\]\[z\] because those will not take into account lightBleed
    - they will likely have to use getLight
    - obv same for sunlight too
- Probably make seperate methods `calculateBleedLighting` and `calculateBleedSkyLighting`
    - These just use lightBleed array
    - maybe make universal calculateLightingBuffer, which takes in light array
        - then both calclight and calcbleedlight wont have dup code
- do NOT clear adj chunk light array when rebuilding. should not ever be affected
- chunk that is modified will have full rebuild, adj ones that have updated lightBleeds will only rebuildmesh as only thing that changes for them is their lightBleed array
- circular lighting should work for any amount of chunk border hops because rec bleed
- basically
    - light: static, within chunk
        - only ever changes when add/remove block
    - lightBleed: dynamic, flows between chunks
        - can change from adj chunk updates

# Basically...
- Step by step for add/remove block
- fullrebuild curr chunk
    - findminmax curr chunk
    - recalclighting for current chunk (static lighting)
    - clear each adj chunk lightbleed (including tl, tr, bl, br)
    - recalcbleedlighting for curr chunk (flow lighting)
        - this scans edge chunk lighting, starts rec lighting (using lightbleed arr)
        - marks chunks that we flow into as dirty (only if any changes were made to their lightBleed arr)
    - rebuildmesh for curr chunk
- next update tick should find next dirty chunk
    - recalcbleedlighting AND rebuildmesh on dirty chunk
        - redundant, but will handle bleed coming from chunks other than main chunk where we did rec bleed into dirty. bc we cleared it, the chunks adj to dirty wont have any light bleeding into dirty
    - mark as no longer dirty
    - repeat for next update tick
- update order does not matter for adj chunks because we can flow between them in calclight
- recalcbleedlighting on dirty chunks can also mark others as dirty, causing chain reaction
    - but only if actual changes made to avoid infinite loops
    - this shouldn't lead to chunks outside the adj being affected
    - only spreading should be happening within the adj
    - also never should cause main (center) chunk to be dirty


- These ideas can apply to world gen light baking
- go through all chunks, generate, calc static lighting, then do bleed lighting
- finally do rebuildmesh
- should lead to perfect lighting on world load

<!-- # Actually... -->
<!-- - Clear adj lightBleed arrays -->
<!-- - Do static lighting on updated chunk -->
<!--     - scan edges too (scan their static lighting) -->
<!-- - go do recalcbleed on all surrounding chunks (including corner chunks) -->
<!-- - don't use rec func that flows, just stay within chunk -->
<!-- - mark all adj as dirty -->
<!-- - on update tick, get next dirty, recalcbleedlight (once again), then rebuildMesh -->
<!-- - for recalcbleedlight, stop if either light or lightBleed greater than prev -->

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
