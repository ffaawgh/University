'use strict';

/**
 * Created by ed on 19.02.17.
 */

const mainCanvas = document.getElementById('mainCanvas');
const mousePositionLabel = document.getElementById('mousePosition');
const addPointButton = document.getElementById('addPoint');

let pointId = 0;
let points = [];

const canvasManager = new CanvasManager(mainCanvas);
const historyManager = new HistoryManager(points);

const updatePointsList = function () {
    let pointsTable = document.getElementById('pointsList');

    // remove children
    while (pointsTable.firstChild) {
        pointsTable.removeChild(pointsTable.firstChild);
    }

    points.forEach(function (point) {
        point.tableRow = addPoint(point);
    });

    updatePointListFooter();
};

const updateTrianglesList = function () {
    let trianglesTable = document.getElementById('trianglesList');

    // remove children
    while (trianglesTable.firstChild) {
        trianglesTable.removeChild(trianglesTable.firstChild);
    }

    let triangles = Triangle.fromPoints(points).sort(Triangle.compare).reverse();
    triangles.forEach(function (triangle) {
        addTriangle(triangle);
        if (!triangle.orthocenter) {
            triangle.tableRow.classList.add('danger');
        }
    });

    triangles.forEach(function (triangle) {
        if (Math.round(triangle.orthocenterDistance) == Math.round(triangles[0].orthocenterDistance)) {
            triangle.tableRow.classList.add('success');
        } else {
            return false;
        }
    });

    updateTrianglesListFooter();
};


const reDrawPoints = function () {
    mainCanvas.width = mainCanvas.parentNode.parentNode.parentNode.clientWidth;

    canvasManager.clear();
    canvasManager.drawGrid();
    points.forEach(function (point) {
        drawPoint(point);
    });
};

const createPoint = function (point) {
    points.push(point);
    point.tableRow = addPoint(point);
    drawPoint(point);

    capture();

    return point;
};

const destroyPoint = function (point) {
    let index = points.indexOf(point);
    points.splice(index, 1);

    removePoint(point);
    reDrawPoints();

    capture();
};


const addPoint = function (point) {
    // add the point to the list
    let row = document.createElement('tr');
    row.point = point;

    // the first column
    let indexCol = document.createElement('td');
    let indexLabel = document.createElement('label');
    indexLabel.innerHTML = ++pointId;
    indexCol.appendChild(indexLabel);
    row.appendChild(indexCol);

    // the second column
    let xCol = document.createElement('td');
    xCol.innerHTML = Math.round(point.x);
    row.appendChild(xCol);

    // the third column
    let yCol = document.createElement('td');
    yCol.innerHTML = Math.round(point.y);
    row.appendChild(yCol);

    // remove button
    let removeCol = document.createElement('td');
    let removeBtn = document.createElement('button');
    removeBtn.type = 'button';
    removeBtn.className = 'btn btn-danger';
    removeBtn.innerHTML = 'Remove';
    removeBtn.point = point;
    removeBtn.addEventListener('click', onRemovePointButtonClick);
    row.addEventListener('mouseover', onPointRowMouseOver);
    row.addEventListener('mouseout', onPointRowMouseOut);
    removeCol.appendChild(removeBtn);
    row.appendChild(removeCol);

    document.getElementById('pointsList').appendChild(row);

    updatePointListFooter();


    return row;
};

const removePoint = function (point) {
    document.getElementById('pointsList').removeChild(point.tableRow);
    delete point.tableRow;
    updatePointListFooter();
};

const resetSize = function () {
    canvasManager.setOffset(0, 0);
    canvasManager.setScale(1, 1);
};

const resize = function (withOrtho) {
    if (points.length >= 3) {
        let xMap = points.map(function (point) {
            return point.x;
        });
        let yMap = points.map(function (point) {
            return point.y;
        });

        if (withOrtho) {
            let triangles = Triangle.fromPoints(points);
            let ortho = triangles.map(function (triangle) {
                return triangle.orthocenter;
            });
            let xOrthoMap = ortho.map(function (point) {
                return point.x;
            });
            let yOrthoMap = ortho.map(function (point) {
                return point.y;
            });
            xMap = xMap.concat(xOrthoMap);
            yMap = yMap.concat(yOrthoMap);
        }

        let xMin = Math.min.apply(Math, xMap);
        let yMin = Math.min.apply(Math, yMap);
        let xMax = Math.max.apply(Math, xMap);
        let yMax = Math.max.apply(Math, yMap);

        let xSize = mainCanvas.width / (xMax - xMin + 20);
        let ySize = mainCanvas.height / (yMax - yMin + 20);

        let size = Math.min(xSize, ySize);

        canvasManager.setOffset(-xMin + 10, -yMin + 10);
        canvasManager.setScale(size, size);
    } else {
        resetSize();
    }
};


const addTriangle = function (triangle) {
    let row = document.createElement('tr');
    row.triangle = triangle;

    // the first column
    let indexCol = document.createElement('td');
    let indexLabel = document.createElement('label');
    indexLabel.innerHTML = document.getElementById('trianglesList').childNodes.length + 1;
    indexCol.appendChild(indexLabel);
    row.appendChild(indexCol);

    // the second column
    let col2 = document.createElement('td');
    col2.innerHTML = `<div>${Math.round(triangle.a.x)}</div><div>${Math.round(triangle.a.y)}</div>`;
    row.appendChild(col2);

    // the third column
    let col3 = document.createElement('td');
    col3.innerHTML = `<div>${Math.round(triangle.b.x)}</div><div>${Math.round(triangle.b.y)}</div>`;
    row.appendChild(col3);

    // the fourth column
    let col4 = document.createElement('td');
    col4.innerHTML = `<div>${Math.round(triangle.c.x)}</div><div>${Math.round(triangle.c.y)}</div>`;
    row.appendChild(col4);

    // the fifth column
    let col5 = document.createElement('td');
    col5.innerHTML = `<div>${Math.round(triangle.orthocenter.x)}</div><div>${Math.round(triangle.orthocenter.y)}</div>`;
    row.appendChild(col5);

    // the sixth column
    let col6 = document.createElement('td');
    col6.innerHTML = `<div>${Math.round(triangle.orthocenterDistance)}</div>`;
    row.appendChild(col6);

    row.addEventListener('mouseover', onTriangleRowMounseOver);
    row.addEventListener('mouseout', onTriangleRowMounseOut);

    triangle.tableRow = row;
    document.getElementById('trianglesList').appendChild(row);
};

const removeTriangle = function (triangle) {
    document.getElementById('trianglesList').removeChild(triangle.tableRow);
    delete triangle.tableRow;
    updateTrianglesListFooter();
};

const drawPoint = function (point) {
    canvasManager.drawPoint({
        point: point,
        color: 'black',
        radius: 3
    });
};

const drawHighlightedPoint = function (point) {
    canvasManager.drawPoint({
        point: point,
        color: 'red',
        radius: 4
    });
};

const drawFadedPoint = function (point) {
    canvasManager.drawPoint({
        point: point,
        color: 'mediumblue',
        radius: 4
    });
};

const drawTriangle = function (triangle) {
    let a = triangle.a;
    let b = triangle.b;
    let c = triangle.c;

    let sideOptions = {lineWidth: 2};

    canvasManager.drawLine(a, b, sideOptions);
    canvasManager.drawLine(a, c, sideOptions);
    canvasManager.drawLine(c, b, sideOptions);

    drawHighlightedPoint(a);
    drawHighlightedPoint(b);
    drawHighlightedPoint(c);

    let orthocenter = triangle.orthocenter;
    orthocenter.x = Math.round(orthocenter.x);
    orthocenter.y = Math.round(orthocenter.y);
    if (orthocenter) {
        let lineOptions = {
            color: 'brown',
            lineWidth: 2
        };

        canvasManager.drawLine(
            {x: 0, y: orthocenter.y},
            orthocenter,
            lineOptions
        );
        canvasManager.drawLine(
            {x: orthocenter.x, y: 0},
            orthocenter,
            lineOptions
        );
        drawFadedPoint(orthocenter);
    }
};

const updatePointListFooter = function () {
    let pointsNumber = document.getElementById('pointsNumber');
    let pointsList = document.getElementById('pointsList');
    pointsNumber.innerHTML = pointsList.childNodes.length.toString();
};

const updateTrianglesListFooter = function () {
    let trianglesNumber = document.getElementById('trianglesNumber');
    let trianglesList = document.getElementById('trianglesList');
    trianglesNumber.innerHTML = trianglesList.childNodes.length.toString();
};

const capture = function () {
    let p = points.map(function (point) {
        return {x: point.x, y: point.y};
    });
    historyManager.capture(p);
};

const undo = function () {
    let newPoints = historyManager.undo();
    if (newPoints) {
        points = newPoints;
        reDrawPoints();
        updateTrianglesList();
        updatePointsList();
    }
};

const redo = function () {
    let newPoints = historyManager.redo();
    if (newPoints) {
        points = newPoints;
        reDrawPoints();
        updateTrianglesList();
        updatePointsList();
    }
};

const reset = function () {
    let newPoints = historyManager.reset();
    if (newPoints) {
        points = newPoints;
        reDrawPoints();
        updateTrianglesList();
        updatePointsList();
    }
};

const clean = function () {
    let newPoints = historyManager.clear();
    if (newPoints) {
        points = newPoints;
        reDrawPoints();
        updateTrianglesList();
        updatePointsList();
    }
};

mainCanvas.addEventListener('click', onCanvasMouseClick);
mainCanvas.addEventListener('mousemove', onCanvasMouseMove);
mainCanvas.addEventListener('mouseleave', onCavasMouseLeave);

addPointButton.addEventListener('click', onAddButtonClick);

document.getElementById('undo').addEventListener('click', onUndo);
document.getElementById('redo').addEventListener('click', onRedo);
document.getElementById('clean').addEventListener('click', onClean);
document.getElementById('reset').addEventListener('click', onReset);

document.getElementById('resize').addEventListener('click', onResize);
document.getElementById('resizeOrtho').addEventListener('click', onResizeOrtho);
document.getElementById('resetSize').addEventListener('click', onResetSize);

setTimeout(reDrawPoints, 10);