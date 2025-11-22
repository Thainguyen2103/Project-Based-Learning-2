#include "Grid.h"
#include <cstdlib>#include <set>

Grid::Grid()
    : mDim(0), mNumColors(0), mCellSize(0), mMapOffsetX(0), mMapOffsetY(0),
    mAlgoType(ALGO_BFS), mFillDir(DIR_FOUR), mIsAnimating(false)
{
    mTargetColor = { 0,0,0,0 };
    mReplacementColor = { 0,0,0,0 };
}

void Grid::recalculateRenderParams() {
    if (mDim == 0) return;
    mCellSize = MAP_AREA_SIZE / mDim;
    mMapOffsetX = (SCREEN_WIDTH - UI_PANEL_WIDTH - MAP_AREA_SIZE) / 2;
    mMapOffsetY = (SCREEN_HEIGHT - MAP_AREA_SIZE) / 2;
}

void Grid::init(int dim, int numColors, AlgorithmType algo, FillDirection fillDir) {
    mDim = dim;
    mNumColors = numColors;
    mAlgoType = algo;
    mFillDir = fillDir;
    mIsAnimating = false;
    recalculateRenderParams();
    generateMap();
    mInitialCells = mCells;
    mPendingChangeCells.clear();
}

void Grid::generateMap() {
    mCells.assign(mDim, std::vector<SDL_Color>(mDim));
    for (int y = 0; y < mDim; ++y) {
        for (int x = 0; x < mDim; ++x) {
            mCells[x][y] = PALETTE[rand() % mNumColors];
        }
    }
}

void Grid::setGrid(const std::vector<std::vector<SDL_Color>>& grid) {
    mCells = grid;
    if (!grid.empty()) {
        mDim = (int)grid.size();
    }
    else {
        mDim = 0;
    }
    recalculateRenderParams();
    mIsAnimating = false;
    mPendingChangeCells.clear();
}

void Grid::setInitialGrid(const std::vector<std::vector<SDL_Color>>& grid) {
    mInitialCells = grid;
}


void Grid::resetToInitial() {
    mCells = mInitialCells;
    mIsAnimating = false;
    mPendingChangeCells.clear();
}

std::vector<std::vector<bool>> Grid::findControlledRegion() const {
    std::vector<std::vector<bool>> visited(mDim, std::vector<bool>(mDim, false));
    if (mCells.empty()) return visited;

    std::queue<std::pair<int, int>> q;
    SDL_Color targetColor = mCells[0][0];

    q.push({ 0, 0 });
    visited[0][0] = true;

    int dx4[] = { 0, 0, 1, -1 };
    int dy4[] = { 1, -1, 0, 0 };
    int dx8[] = { 0, 0, 1, -1, 1, 1, -1, -1 };
    int dy8[] = { 1, -1, 0, 0, 1, -1, 1, -1 };
    int directions = (mFillDir == DIR_FOUR) ? 4 : 8;
    int* dx = (mFillDir == DIR_FOUR) ? dx4 : dx8;
    int* dy = (mFillDir == DIR_FOUR) ? dy4 : dy8;

    while (!q.empty()) {
        std::pair<int, int> curr = q.front(); q.pop();
        int x = curr.first; int y = curr.second;

        for (int i = 0; i < directions; ++i) {
            int nx = x + dx[i]; int ny = y + dy[i];
            if (nx >= 0 && nx < mDim && ny >= 0 && ny < mDim &&
                !visited[nx][ny] && areColorsEqual(mCells[nx][ny], targetColor)) {
                visited[nx][ny] = true;
                q.push({ nx, ny });
            }
        }
    }
    return visited;
}

void Grid::draw(SDL_Renderer* renderer, bool showControlledRegion, bool showStartHint, bool showEventWarning) const {
    drawInViewport(renderer, mMapOffsetX, mMapOffsetY, mCellSize);

    if (showControlledRegion) {
        auto controlledRegion = findControlledRegion();
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 100);
        for (int y = 0; y < mDim; ++y) {
            for (int x = 0; x < mDim; ++x) {
                if (controlledRegion[x][y]) {
                    SDL_Rect cellRect = { mMapOffsetX + x * mCellSize, mMapOffsetY + y * mCellSize, mCellSize, mCellSize };
                    SDL_RenderDrawRect(renderer, &cellRect);
                }
            }
        }
    }

    if (showStartHint && SDL_GetTicks() % 1000 < 500) {
        SDL_Rect startCellRect = { mMapOffsetX, mMapOffsetY, mCellSize, mCellSize };
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128);
        SDL_RenderFillRect(renderer, &startCellRect);
    }

    if (showEventWarning && !mPendingChangeCells.empty()) {
        if (SDL_GetTicks() % 500 < 250) {
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 100);
            for (const auto& cellPos : mPendingChangeCells) {
                SDL_Rect highlightRect = {
                    mMapOffsetX + cellPos.first * mCellSize,
                    mMapOffsetY + cellPos.second * mCellSize,
                    mCellSize, mCellSize
                };
                SDL_RenderFillRect(renderer, &highlightRect);
            }
        }
    }
}

void Grid::drawInViewport(SDL_Renderer* renderer, int startX, int startY, int cellSize) const {
    SDL_Rect cellRect = { 0, 0, cellSize, cellSize };
    for (int y = 0; y < mDim; ++y) {
        for (int x = 0; x < mDim; ++x) {
            cellRect.x = startX + x * cellSize;
            cellRect.y = startY + y * cellSize;
            SDL_SetRenderDrawColor(renderer, mCells[x][y].r, mCells[x][y].g, mCells[x][y].b, 255);
            SDL_RenderFillRect(renderer, &cellRect);
        }
    }
}


bool Grid::startFloodFill(const SDL_Color& newColor) {
    if (mCells.empty() || areColorsEqual(mCells[0][0], newColor)) return false;


    mTargetColor = mCells[0][0];
    mReplacementColor = newColor;
    mIsAnimating = true;

    if (mAlgoType == ALGO_UNION_FIND) {
        DSU dsu(mDim * mDim);
        for (int y = 0; y < mDim; ++y) {
            for (int x = 0; x < mDim; ++x) {
                if (areColorsEqual(mCells[x][y], mTargetColor)) {
                    if (x + 1 < mDim && areColorsEqual(mCells[x + 1][y], mTargetColor)) dsu.unite(y * mDim + x, y * mDim + x + 1);
                    if (y + 1 < mDim && areColorsEqual(mCells[x][y + 1], mTargetColor)) dsu.unite(y * mDim + x, (y + 1) * mDim + x);
                    if (mFillDir == DIR_EIGHT) {
                        if (x + 1 < mDim && y + 1 < mDim && areColorsEqual(mCells[x + 1][y + 1], mTargetColor)) dsu.unite(y * mDim + x, (y + 1) * mDim + x + 1);
                        if (x - 1 >= 0 && y + 1 < mDim && areColorsEqual(mCells[x - 1][y + 1], mTargetColor)) dsu.unite(y * mDim + x, (y + 1) * mDim + x - 1);
                    }
                }
            }
        }
        int startRoot = dsu.find(0);
        for (int y = 0; y < mDim; ++y) {
            for (int x = 0; x < mDim; ++x) {
                if (dsu.find(y * mDim + x) == startRoot) mCells[x][y] = mReplacementColor;
            }
        }
        mIsAnimating = false;
    }
    else {
        while (!mBfsQueue.empty()) mBfsQueue.pop();
        while (!mDfsStack.empty()) mDfsStack.pop();
        while (!mScanlineStack.empty()) mScanlineStack.pop();

        if (mAlgoType == ALGO_BFS) mBfsQueue.push({ 0, 0 });
        else if (mAlgoType == ALGO_DFS) mDfsStack.push({ 0, 0 });
        else if (mAlgoType == ALGO_SCANLINE) mScanlineStack.push({ 0, 0 });
    }
    return true;
}

bool Grid::animateFillStep() {
    if (!mIsAnimating) return true;

    int steps = 0;
    bool animationEnded = false;
    switch (mAlgoType) {
    case ALGO_BFS:
    case ALGO_DFS: {
        auto& q_or_s = (mAlgoType == ALGO_BFS) ? (void*&)mBfsQueue : (void*&)mDfsStack;
        while ((mAlgoType == ALGO_BFS ? !mBfsQueue.empty() : !mDfsStack.empty()) && steps < STEPS_PER_FRAME) {
            std::pair<int, int> curr;
            if (mAlgoType == ALGO_BFS) { curr = mBfsQueue.front(); mBfsQueue.pop(); }
            else { curr = mDfsStack.top(); mDfsStack.pop(); }
            int x = curr.first, y = curr.second;
            if (x < 0 || x >= mDim || y < 0 || y >= mDim || !areColorsEqual(mCells[x][y], mTargetColor)) continue;
            mCells[x][y] = mReplacementColor;

            int dx4[] = { 1, -1, 0, 0 };
            int dy4[] = { 0, 0, 1, -1 };
            int dx8[] = { 1, -1, 0, 0, 1, -1, 1, -1 };
            int dy8[] = { 0, 0, 1, -1, 1, 1, -1, -1 };
            int directions = (mFillDir == DIR_FOUR) ? 4 : 8;
            int* dx = (mFillDir == DIR_FOUR) ? dx4 : dx8;
            int* dy = (mFillDir == DIR_FOUR) ? dy4 : dy8;

            for (int i = 0; i < directions; ++i) {
                if (mAlgoType == ALGO_BFS) mBfsQueue.push({ x + dx[i], y + dy[i] }); else mDfsStack.push({ x + dx[i], y + dy[i] });
            }
            steps++;
        }
        if (mAlgoType == ALGO_BFS ? mBfsQueue.empty() : mDfsStack.empty()) animationEnded = true;
    } break;
    case ALGO_SCANLINE:
        while (!mScanlineStack.empty() && steps < 1) {
            std::pair<int, int> curr = mScanlineStack.top(); mScanlineStack.pop();
            int x = curr.first, y = curr.second;
            if (y < 0 || y >= mDim || !areColorsEqual(mCells[x][y], mTargetColor)) continue;
            int west = x; while (west > 0 && areColorsEqual(mCells[west - 1][y], mTargetColor)) west--;
            int east = x; while (east < mDim - 1 && areColorsEqual(mCells[east + 1][y], mTargetColor)) east++;
            for (int i = west; i <= east; ++i) {
                mCells[i][y] = mReplacementColor;
                if (y > 0 && areColorsEqual(mCells[i][y - 1], mTargetColor)) mScanlineStack.push({ i, y - 1 });
                if (y < mDim - 1 && areColorsEqual(mCells[i][y + 1], mTargetColor)) mScanlineStack.push({ i, y + 1 });
            }
            steps++;
        }
        if (mScanlineStack.empty()) animationEnded = true;
        break;
    default:
        animationEnded = true;
        break;
    }

    if (animationEnded) {
        mIsAnimating = false;
        return true;
    }
    return false;
}

bool Grid::isWinConditionMet() const {
    if (mCells.empty()) return false;
    const SDL_Color& firstColor = mCells[0][0];
    for (int y = 0; y < mDim; ++y) {
        for (int x = 0; x < mDim; ++x) {
            if (!areColorsEqual(mCells[x][y], firstColor)) {
                return false;
            }
        }
    }
    return true;
}

PredictionResult Grid::predictProgress() const {
    PredictionResult result;
    if (mCells.empty()) {
        return { true, 0, 0 };
    }

    std::set<SDL_Color, ColorComparator> remainingColorsSet;
    for (int y = 0; y < mDim; ++y) {
        for (int x = 0; x < mDim; ++x) {
            remainingColorsSet.insert(mCells[x][y]);
        }
    }

    SDL_Color playerColor = mCells[0][0];
    remainingColorsSet.erase(playerColor);

    result.predictedSteps = (int)remainingColorsSet.size();
    result.remainingColors = (int)remainingColorsSet.size();
    return result;
}

int Grid::countConnectedRegion(const std::vector<std::vector<SDL_Color>>& grid) const {
    int dim = (int)grid.size();
    if (dim == 0) return 0;
    std::vector<std::vector<bool>> visited(dim, std::vector<bool>(dim, false));
    std::queue<std::pair<int, int>> q;
    int count = 0;
    SDL_Color targetColor = grid[0][0];

    q.push({ 0,0 });
    visited[0][0] = true;
    while (!q.empty()) {
        std::pair<int, int> curr = q.front(); q.pop();
        count++;
        int x = curr.first; int y = curr.second;

        int dx4[] = { 0,0,1,-1 };
        int dy4[] = { 1,-1,0,0 };
        int dx8[] = { 0, 0, 1, -1, 1, 1, -1, -1 };
        int dy8[] = { 1, -1, 0, 0, 1, -1, 1, -1 };
        int directions = (mFillDir == DIR_FOUR) ? 4 : 8;
        int* dx = (mFillDir == DIR_FOUR) ? dx4 : dx8;
        int* dy = (mFillDir == DIR_FOUR) ? dy4 : dy8;

        for (int i = 0; i < directions; ++i) {
            int nx = x + dx[i]; int ny = y + dy[i];
            if (nx >= 0 && nx < dim && ny >= 0 && ny < dim && !visited[nx][ny] && areColorsEqual(grid[nx][ny], targetColor)) {
                visited[nx][ny] = true;
                q.push({ nx,ny });
            }
        }
    }
    return count;
}


SDL_Color Grid::findBestMove() const {
    SDL_Color bestColor = mCells[0][0];
    int maxConnected = -1;
    SDL_Color startColor = mCells[0][0];

    for (int i = 0; i < mNumColors; ++i) {
        SDL_Color currentColor = PALETTE[i];
        if (areColorsEqual(currentColor, startColor)) continue;

        std::vector<std::vector<SDL_Color>> tempGrid = mCells;
        std::queue<std::pair<int, int>> flood_q;
        flood_q.push({ 0, 0 });
        std::vector<std::vector<bool>> visited(mDim, std::vector<bool>(mDim, false));
        visited[0][0] = true;

        while (!flood_q.empty()) {
            std::pair<int, int> p = flood_q.front(); flood_q.pop();
            tempGrid[p.first][p.second] = currentColor;

            int dx4[] = { 0, 0, 1, -1 };
            int dy4[] = { 1, -1, 0, 0 };
            int dx8[] = { 0, 0, 1, -1, 1, 1, -1, -1 };
            int dy8[] = { 1, -1, 0, 0, 1, -1, 1, -1 };
            int directions = (mFillDir == DIR_FOUR) ? 4 : 8;
            int* dx = (mFillDir == DIR_FOUR) ? dx4 : dx8;
            int* dy = (mFillDir == DIR_FOUR) ? dy4 : dy8;

            for (int k = 0; k < directions; ++k) {
                int nx = p.first + dx[k];
                int ny = p.second + dy[k];
                if (nx >= 0 && nx < mDim && ny >= 0 && ny < mDim && !visited[nx][ny] && areColorsEqual(mCells[nx][ny], startColor)) {
                    visited[nx][ny] = true;
                    flood_q.push({ nx, ny });
                }
            }
        }

        int connectedSize = countConnectedRegion(tempGrid);

        if (connectedSize > maxConnected) {
            maxConnected = connectedSize;
            bestColor = currentColor;
        }
    }
    return bestColor;
}

void Grid::selectRandomCellsForChangeEvent(int count) {
    mPendingChangeCells.clear();
    std::vector<std::pair<int, int>> allCells;
    for (int y = 0; y < mDim; ++y) {
        for (int x = 0; x < mDim; ++x) {
            allCells.push_back({ x, y });
        }
    }
    if (allCells.empty()) return;

    std::random_shuffle(allCells.begin(), allCells.end());

    count = std::min(count, (int)allCells.size());

    for (int i = 0; i < count; ++i) {
        mPendingChangeCells.push_back(allCells[i]);
    }
}

bool Grid::executeColorChangeEvent() {
    if (mPendingChangeCells.empty()) return false;

    for (const auto& cell : mPendingChangeCells) {
        int x = cell.first;
        int y = cell.second;
        SDL_Color currentColor = mCells[x][y];
        SDL_Color newColor;
        do {
            newColor = PALETTE[rand() % mNumColors];
        } while (areColorsEqual(newColor, currentColor));
        mCells[x][y] = newColor;
    }

    mPendingChangeCells.clear();
    return true;
}

SDL_Color Grid::getColorFromClick(int mX, int mY) const {
    if (!isClickInBounds(mX, mY)) {
        return { 0,0,0,0 };
    }
    int gridX = (mX - mMapOffsetX) / mCellSize;
    int gridY = (mY - mMapOffsetY) / mCellSize;
    if (gridX >= 0 && gridX < mDim && gridY >= 0 && gridY < mDim) {
        return mCells[gridX][gridY];
    }
    return { 0,0,0,0 };
}

bool Grid::isClickInBounds(int mX, int mY) const {
    if (mCellSize == 0) return false;
    int gridX = (mX - mMapOffsetX) / mCellSize;
    int gridY = (mY - mMapOffsetY) / mCellSize;
    return gridX >= 0 && gridX < mDim && gridY >= 0 && gridY < mDim;
}