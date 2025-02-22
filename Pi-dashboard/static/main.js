document.addEventListener("DOMContentLoaded", function () {
    if (typeof Plotly === "undefined") {
        console.error("Plotly is not loaded properly.");
        return;
    }

    console.log("Plotly loaded successfully!");

    const data = [{
        x: [1, 2, 3, 4, 5],
        y: [2, 4, 1, 3, 5],
        type: "scatter"
    }];

    const layout = {
        title: "Sample Scatter Plot",
        xaxis: { title: "X-axis" },
        yaxis: { title: "Y-axis" }
    };

    let graphDiv = document.getElementById("myGraph");
    if (graphDiv) {
        Plotly.newPlot(graphDiv, data, layout);
        console.log("Plotly graph should be displayed.");
    } else {
        console.error("Element #myGraph not found!");
    }
});
