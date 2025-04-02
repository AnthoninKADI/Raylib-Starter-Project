#version 430 core

layout (local_size_x = 1, local_size_y = 1) in;

struct PositionComponent {
    vec2 position;
};

layout (std140, binding = 0) buffer enemyPositions {
    PositionComponent enemyPositions[];
};

layout (std140, binding = 1) buffer playerPosition {
    PositionComponent playerPosition[];
};

void main() {
    uint id = gl_GlobalInvocationID.x;

    vec2 direction = normalize(playerPosition[0].position - enemyPositions[id].position);

    enemyPositions[id].position += direction * 0.1;
}
