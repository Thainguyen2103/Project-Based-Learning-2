#include "AISolver.h"
#include "DSU.h"
#include "ResourceManager.h" 
#include <set>
#include <algorithm>
AISolver::AISolver()
    : mAlgoType(ALGO_BFS), mFillDir(DIR_FOUR), mNumColors(6),
    mMoves(0),    mIsDone(false), mIsAnimating(false), mElapsedTime(std::chrono::duration<double>::zero())
{
}

void AISolver::init(AlgorithmType algo, FillDirection fillDir, const std::vector<std::vector<SDL_Color>>& masterGrid, int numColors) {
    mAlgoType = algo;
    mFillDir = fillDir;
    mNumColors = numColors;
    mIsDone = false;
    mIsAnimating = false;
    mMoves = 0;    mElapsedTime = std::chrono::duration<double>::zero();
    mStartTime = std::chrono::steady_clock::now();

    mGrid.init((int)masterGrid.size(), numColors, algo, fillDir);
    mGrid.setGrid(masterGrid);
}

std::string AISolver::getName() const {
    switch (mAlgoType) {
    case ALGO_BFS: return "BFS";
    case ALGO_DFS: return "DFS";
    case ALGO_SCANLINE: return "Scanline";
    case ALGO_UNION_FIND: return "Union-Find";
    }
    return "Unknown";
}

void AISolver::update() {
    if (mIsDone) return;

    mElapsedTime = std::chrono::steady_clock::now() - mStartTime;

    if (mIsAnimating) {
        animateFillStep();
    }
    else {
        checkWinCondition();
        if (!mIsDone) {
            SDL_Color nextMove = findBestNextMove();
            startFloodFill(nextMove);
        }
    }
}

void AISolver::draw(SDL_Renderer* renderer, SDL_Rect viewport, TTF_Font* font, TTF_Font* smallFont, ResourceManager* res) {
    SDL_SetRenderDrawColor(renderer, UI_BG_COLOR.r, UI_BG_COLOR.g, UI_BG_COLOR.b, 255);
    SDL_RenderFillRect(renderer, &viewport);

    if (mGrid.getDim() == 0) return;

    int mapSize = std::min(viewport.w - 20, viewport.h - 80);
    int cellSize = std::max(1, mapSize / mGrid.getDim());
    int renderMapSize = cellSize * mGrid.getDim();
    int offsetX = viewport.x + (viewport.w - renderMapSize) / 2;
    int offsetY = viewport.y + 40;

    mGrid.drawInViewport(renderer, offsetX, offsetY, cellSize);

    drawText(renderer, getName(), viewport.x + viewport.w / 2, viewport.y + 25, font, PALETTE[(int)mAlgoType]);
    std::stringstream ss;
    ss << res->getText("ai_solving_moves") << " " << mMoves;
    drawText(renderer, ss.str(), viewport.x + viewport.w / 2 - 80, viewport.y + viewport.h - 25, smallFont, TEXT_WHITE);

    ss.str("");
    ss << std::fixed << std::setprecision(2) << mElapsedTime.count() << "s";
    drawText(renderer, res->getText("ai_solving_time") + " " + ss.str(), viewport.x + viewport.w / 2 + 80, viewport.y + viewport.h - 25, smallFont, TEXT_WHITE);

    if (mIsDone) {
        drawText(renderer, res->getText("ai_solving_done"), viewport.x + viewport.w / 2, offsetY + renderMapSize / 2, font, BUTTON_YELLOW);
    }
}

int AISolver::countConnectedRegion(const std::vector<std::vector<SDL_Color>>& grid, FillDirection fillDir) {
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
        int directions = (fillDir == DIR_FOUR) ? 4 : 8;
        int* dx = (fillDir == DIR_FOUR) ? dx4 : dx8;
        int* dy = (fillDir == DIR_FOUR) ? dy4 : dy8;

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

SDL_Color AISolver::findBestNextMove() const {
    SDL_Color bestColor = mGrid.getCells()[0][0];
    int maxConnected = -1;
    SDL_Color startColor = mGrid.getCells()[0][0];
    int current_dim = mGrid.getDim();

    for (int i = 0; i < mNumColors; ++i) {
        SDL_Color currentColor = PALETTE[i];
        if (areColorsEqual(currentColor, startColor)) continue;

        std::vector<std::vector<SDL_Color>> tempGrid = mGrid.getCells();
        std::queue<std::pair<int, int>> flood_q;
        flood_q.push({ 0,0 });
        std::vector<std::vector<bool>> visited(current_dim, std::vector<bool>(current_dim, false));
        visited[0][0] = true;

        while (!flood_q.empty()) {
            std::pair<int, int> p = flood_q.front();
            flood_q.pop();
            tempGrid[p.first][p.second] = currentColor;

            int dx4[] = { 0,0,1,-1 };
            int dy4[] = { 1,-1,0,0 };
            int dx8[] = { 0, 0, 1, -1, 1, 1, -1, -1 };
            int dy8[] = { 1, -1, 0, 0, 1, -1, 1, -1 };
            int directions = (mFillDir == DIR_FOUR) ? 4 : 8;
            int* dx = (mFillDir == DIR_FOUR) ? dx4 : dx8;
            int* dy = (mFillDir == DIR_FOUR) ? dy4 : dy8;

            for (int k = 0; k < directions; ++k) {
                int nx = p.first + dx[k];
                int ny = p.second + dy[k];
                if (nx >= 0 && nx < current_dim && ny >= 0 && ny < current_dim && !visited[nx][ny] && areColorsEqual(mGrid.getCells()[nx][ny], startColor)) {
                    visited[nx][ny] = true;
                    flood_q.push({ nx,ny });
                }
            }
        }

        int connectedSize = countConnectedRegion(tempGrid, mFillDir);
        if (connectedSize > maxConnected) {
            maxConnected = connectedSize;
            bestColor = currentColor;
        }
    }
    return bestColor;
}

void AISolver::startFloodFill(const SDL_Color& newColor) {
    if (areColorsEqual(mGrid.getCells()[0][0], newColor)) return;

    mMoves++;

    mTargetColor = mGrid.getCells()[0][0];
    mReplacementColor = newColor;
    mIsAnimating = true;
    int current_dim = mGrid.getDim();

    auto& cells = mGrid.getCells_NonConst();

    if (mAlgoType == ALGO_UNION_FIND) {
        DSU dsu(current_dim * current_dim);
        for (int y = 0; y < current_dim; ++y) {
            for (int x = 0; x < current_dim; ++x) {
                if (areColorsEqual(cells[x][y], mTargetColor)) {
                    if (x + 1 < current_dim && areColorsEqual(cells[x + 1][y], mTargetColor)) dsu.unite(y * current_dim + x, y * current_dim + x + 1);
                    if (y + 1 < current_dim && areColorsEqual(cells[x][y + 1], mTargetColor)) dsu.unite(y * current_dim + x, (y + 1) * current_dim + x);
                    if (mFillDir == DIR_EIGHT) {
                        if (x + 1 < current_dim && y + 1 < current_dim && areColorsEqual(cells[x + 1][y + 1], mTargetColor)) dsu.unite(y * current_dim + x, (y + 1) * current_dim + x + 1);
                        if (x - 1 >= 0 && y + 1 < current_dim && areColorsEqual(cells[x - 1][y + 1], mTargetColor)) dsu.unite(y * current_dim + x, (y + 1) * current_dim + x - 1);
                    }
                }
            }
        }
        int startRoot = dsu.find(0);
        for (int y = 0; y < current_dim; ++y) {
            for (int x = 0; x < current_dim; ++x) {
                if (dsu.find(y * current_dim + x) == startRoot) cells[x][y] = mReplacementColor;
            }
        }
        mIsAnimating = false;
    }
    else {
        while (!mBfsQueue.empty()) mBfsQueue.pop();
        while (!mDfsStack.empty()) mDfsStack.pop();
        while (!mScanlineStack.empty()) mScanlineStack.pop();

        if (mAlgoType == ALGO_BFS) mBfsQueue.push({ 0,0 });
        else if (mAlgoType == ALGO_DFS) mDfsStack.push({ 0,0 });
        else if (mAlgoType == ALGO_SCANLINE) mScanlineStack.push({ 0,0 });
    }
}

void AISolver::checkWinCondition() {
    if (mIsDone) return;
    if (mGrid.isWinConditionMet()) {
        mIsDone = true;
    }
}

void AISolver::animateFillStep() {
    if (!mIsAnimating) return;
    int steps = 0;
    int dim = mGrid.getDim();
    auto& cells = mGrid.getCells_NonConst();

    const int BFS_DFS_SPEED = 100;
    switch (mAlgoType) {
    case ALGO_BFS: case ALGO_DFS: {
        while ((mAlgoType == ALGO_BFS ? !mBfsQueue.empty() : !mDfsStack.empty()) && steps < BFS_DFS_SPEED) {
            std::pair<int, int> curr;
            if (mAlgoType == ALGO_BFS) { curr = mBfsQueue.front(); mBfsQueue.pop(); }
            else { curr = mDfsStack.top(); mDfsStack.pop(); }

            int x = curr.first, y = curr.second;
            if (x < 0 || x >= dim || y < 0 || y >= dim || !areColorsEqual(cells[x][y], mTargetColor)) continue;
            cells[x][y] = mReplacementColor;

            int dx4[] = { 1,-1,0,0 };
            int dy4[] = { 0,0,1,-1 };
            int dx8[] = { 1, -1, 0, 0, 1, -1, 1, -1 };
            int dy8[] = { 0, 0, 1, -1, 1, 1, -1, -1 };
            int directions = (mFillDir == DIR_FOUR) ? 4 : 8;
            int* dx = (mFillDir == DIR_FOUR) ? dx4 : dx8;
            int* dy = (mFillDir == DIR_FOUR) ? dy4 : dy8;

            for (int i = 0; i < directions; ++i) {
                if (mAlgoType == ALGO_BFS) mBfsQueue.push({ x + dx[i],y + dy[i] });
                else mDfsStack.push({ x + dx[i],y + dy[i] });
            }
            steps++;
        }
        if (mAlgoType == ALGO_BFS ? mBfsQueue.empty() : mDfsStack.empty()) { mIsAnimating = false; }
    } break;

    case ALGO_SCANLINE: {
        while (!mScanlineStack.empty() && steps < STEPS_PER_FRAME * 2) {            std::pair<int, int> curr = mScanlineStack.top(); mScanlineStack.pop();
            int x1 = curr.first, y = curr.second;
            if (y < 0 || y >= dim || !areColorsEqual(cells[x1][y], mTargetColor)) continue;

            while (x1 > 0 && areColorsEqual(cells[x1 - 1][y], mTargetColor)) x1--;
            int x2 = curr.first;
            while (x2 < dim - 1 && areColorsEqual(cells[x2 + 1][y], mTargetColor)) x2++;

            for (int x = x1; x <= x2; ++x) cells[x][y] = mReplacementColor;

            for (int y_offset = -1; y_offset <= 1; y_offset += 2) {
                int next_y = y + y_offset;
                if (next_y < 0 || next_y >= dim) continue;
                for (int x = x1; x <= x2; ++x) {
                    if (areColorsEqual(cells[x][next_y], mTargetColor)) {
                        if (x == x1 || !areColorsEqual(cells[x - 1][next_y], mTargetColor)) {
                            mScanlineStack.push({ x, next_y });
                        }
                    }
                }
            }

            if (mFillDir == DIR_EIGHT) {
                for (int i = x1; i <= x2; ++i) {
                    if (i > 0 && y > 0 && areColorsEqual(cells[i - 1][y - 1], mTargetColor)) mScanlineStack.push({ i - 1, y - 1 });
                    if (i < dim - 1 && y > 0 && areColorsEqual(cells[i + 1][y - 1], mTargetColor)) mScanlineStack.push({ i + 1, y - 1 });
                    if (i > 0 && y < dim - 1 && areColorsEqual(cells[i - 1][y + 1], mTargetColor)) mScanlineStack.push({ i - 1, y + 1 });
                    if (i < dim - 1 && y < dim - 1 && areColorsEqual(cells[i + 1][y + 1], mTargetColor)) mScanlineStack.push({ i + 1, y + 1 });
                }
            }
            steps++;
        }
        if (mScanlineStack.empty()) { mIsAnimating = false; }
    } break;

    default:
        mIsAnimating = false;
        break;
    }
}