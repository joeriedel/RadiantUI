/**
 * Adobe Edge: symbol definitions
 */
(function($, Edge, compId){
//images folder
var im='images/';

var fonts = {};
var opts = {};
var resources = [
];
var symbols = {
"stage": {
    version: "3.0.0",
    minimumCompatibleVersion: "3.0.0",
    build: "3.0.0.322",
    baseState: "Base State",
    scaleToFit: "width",
    centerStage: "both",
    initialState: "Base State",
    gpuAccelerate: false,
    resizeInstances: false,
    content: {
            dom: [
            {
                id: 'ControlButton',
                type: 'rect',
                rect: ['329', '141','auto','auto','auto', 'auto'],
                title: 'Open Door'
            }],
            symbolInstances: [
            {
                id: 'ControlButton',
                symbolName: 'MainMenuButton_S',
                autoPlay: {

                }
            }
            ]
        },
    states: {
        "Base State": {
            "${_Stage}": [
                ["color", "background-color", 'rgba(23,23,23,1.00)'],
                ["style", "overflow", 'hidden'],
                ["style", "height", '334px'],
                ["style", "width", '550px']
            ],
            "${_ControlButton}": [
                ["style", "left", '165px'],
                ["style", "top", '141px']
            ]
        }
    },
    timelines: {
        "Default Timeline": {
            fromState: "Base State",
            toState: "",
            duration: 0,
            autoPlay: true,
            timeline: [
                { id: "eid9", tween: [ "style", "${_ControlButton}", "top", '141px', { fromValue: '141px'}], position: 0, duration: 0 },
                { id: "eid8", tween: [ "style", "${_ControlButton}", "left", '165px', { fromValue: '165px'}], position: 0, duration: 0 }            ]
        }
    }
},
"MainMenuButton_S": {
    version: "3.0.0",
    minimumCompatibleVersion: "3.0.0",
    build: "3.0.0.322",
    baseState: "Base State",
    scaleToFit: "none",
    centerStage: "none",
    initialState: "Base State",
    gpuAccelerate: false,
    resizeInstances: true,
    content: {
            dom: [
                {
                    rect: ['0px', '0px', '220px', '52px', 'auto', 'auto'],
                    borderRadius: ['10px', '10px', '10px', '10px'],
                    id: 'Normal',
                    stroke: [0, 'rgb(0, 0, 0)', 'none'],
                    type: 'rect',
                    fill: ['rgba(211,203,115,1.00)']
                },
                {
                    rect: ['0px', '0px', '224px', '65px', 'auto', 'auto'],
                    borderRadius: ['10px', '10px', '10px', '10px'],
                    type: 'rect',
                    id: 'Highlight',
                    stroke: [0, 'rgb(0, 0, 0)', 'none'],
                    display: 'none',
                    fill: ['rgba(178,169,192,1.00)']
                },
                {
                    rect: ['0px', '0px', '224px', '65px', 'auto', 'auto'],
                    borderRadius: ['10px', '10px', '10px', '10px'],
                    type: 'rect',
                    id: 'Pressed',
                    stroke: [0, 'rgb(0, 0, 0)', 'none'],
                    display: 'none',
                    fill: ['rgba(138,117,172,1.00)']
                },
                {
                    rect: ['0px', '15px', '220px', '37px', 'auto', 'auto'],
                    align: 'center',
                    font: ['Arial, Helvetica, sans-serif', 24, 'rgba(74,73,68,1.00)', '400', 'none', 'normal'],
                    id: 'Label',
                    text: 'Button',
                    textShadow: ['rgba(47,47,47,1.00)', 0, 0, 8],
                    type: 'text'
                },
                {
                    rect: ['0px', '0px', '220px', '52px', 'auto', 'auto'],
                    id: 'EventRect',
                    stroke: [0, 'rgb(0, 0, 0)', 'none'],
                    type: 'rect',
                    fill: ['rgba(138,117,172,1)']
                }
            ],
            symbolInstances: [
            ]
        },
    states: {
        "Base State": {
            "${_Label}": [
                ["subproperty", "textShadow.blur", '8px'],
                ["subproperty", "textShadow.offsetH", '0px'],
                ["color", "color", 'rgba(74,73,68,1.00)'],
                ["subproperty", "textShadow.offsetV", '0px'],
                ["style", "left", '0px'],
                ["style", "width", '220px'],
                ["style", "top", '15px'],
                ["style", "text-align", 'center'],
                ["style", "overflow", 'visible'],
                ["style", "height", '37px'],
                ["subproperty", "textShadow.color", 'rgba(47,47,47,1.00)'],
                ["style", "text-decoration", 'none'],
                ["style", "font-style", 'normal']
            ],
            "${_Highlight}": [
                ["color", "background-color", 'rgba(224,220,160,1.00)'],
                ["style", "height", '52px'],
                ["style", "display", 'none'],
                ["style", "top", '0px'],
                ["style", "left", '0px'],
                ["style", "width", '220px']
            ],
            "${_Normal}": [
                ["color", "background-color", 'rgba(211,203,115,1.00)'],
                ["style", "height", '52px'],
                ["style", "top", '0px'],
                ["style", "left", '0px'],
                ["style", "width", '220px']
            ],
            "${_Pressed}": [
                ["color", "background-color", 'rgba(204,194,90,1.00)'],
                ["style", "height", '52px'],
                ["style", "display", 'none'],
                ["style", "top", '0px'],
                ["style", "left", '0px'],
                ["style", "width", '220px']
            ],
            "${symbolSelector}": [
                ["style", "height", '52px'],
                ["style", "width", '220px']
            ],
            "${_EventRect}": [
                ["style", "height", '52px'],
                ["style", "opacity", '0'],
                ["style", "width", '220px']
            ]
        }
    },
    timelines: {
        "Default Timeline": {
            fromState: "Base State",
            toState: "",
            duration: 2000,
            autoPlay: false,
            labels: {
                "Normal": 0,
                "Highlight": 1000,
                "Pressed": 2000
            },
            timeline: [
                { id: "eid7", tween: [ "style", "${_Pressed}", "width", '220px', { fromValue: '220px'}], position: 0, duration: 0 },
                { id: "eid32", tween: [ "color", "${_Pressed}", "background-color", 'rgba(204,194,90,1.00)', { animationColorSpace: 'RGB', valueTemplate: undefined, fromValue: 'rgba(204,194,90,1.00)'}], position: 0, duration: 0 },
                { id: "eid17", tween: [ "style", "${_Pressed}", "display", 'none', { fromValue: 'none'}], position: 0, duration: 0 },
                { id: "eid15", tween: [ "style", "${_Pressed}", "display", 'none', { fromValue: 'none'}], position: 1000, duration: 0 },
                { id: "eid18", tween: [ "style", "${_Pressed}", "display", 'block', { fromValue: 'none'}], position: 2000, duration: 0 },
                { id: "eid6", tween: [ "style", "${_Pressed}", "height", '52px', { fromValue: '52px'}], position: 0, duration: 0 },
                { id: "eid5", tween: [ "style", "${_Highlight}", "width", '220px', { fromValue: '220px'}], position: 0, duration: 0 },
                { id: "eid16", tween: [ "style", "${_Highlight}", "display", 'none', { fromValue: 'none'}], position: 0, duration: 0 },
                { id: "eid14", tween: [ "style", "${_Highlight}", "display", 'block', { fromValue: 'none'}], position: 1000, duration: 0 },
                { id: "eid19", tween: [ "style", "${_Highlight}", "display", 'none', { fromValue: 'block'}], position: 2000, duration: 0 },
                { id: "eid31", tween: [ "color", "${_Highlight}", "background-color", 'rgba(224,220,160,1.00)', { animationColorSpace: 'RGB', valueTemplate: undefined, fromValue: 'rgba(224,220,160,1.00)'}], position: 0, duration: 0 },
                { id: "eid4", tween: [ "style", "${_Highlight}", "height", '52px', { fromValue: '52px'}], position: 0, duration: 0 }            ]
        }
    }
}
};


Edge.registerCompositionDefn(compId, symbols, fonts, resources, opts);

/**
 * Adobe Edge DOM Ready Event Handler
 */
$(window).ready(function() {
     Edge.launchComposition(compId);
});
})(jQuery, AdobeEdge, "EDGE-1952032168");
