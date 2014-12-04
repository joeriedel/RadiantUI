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
                id: 'RoundRect',
                type: 'rect',
                rect: ['77px', '72px','80px','254px','auto', 'auto'],
                borderRadius: ["10px", "10px", "10px", "10px"],
                fill: ["rgba(255,0,0,0.54)"],
                stroke: [0,"rgb(0, 0, 0)","none"]
            },
            {
                id: 'RoundRectCopy',
                type: 'rect',
                rect: ['181px', '72px','80px','254px','auto', 'auto'],
                borderRadius: ["10px", "10px", "10px", "10px"],
                fill: ["rgba(255,151,0,0.54)"],
                stroke: [0,"rgb(0, 0, 0)","none"]
            },
            {
                id: 'RoundRectCopy2',
                type: 'rect',
                rect: ['287px', '72px','80px','254px','auto', 'auto'],
                borderRadius: ["10px", "10px", "10px", "10px"],
                fill: ["rgba(47,255,0,0.54)"],
                stroke: [0,"rgb(0, 0, 0)","none"]
            },
            {
                id: 'RoundRectCopy3',
                type: 'rect',
                rect: ['393px', '72px','80px','254px','auto', 'auto'],
                borderRadius: ["10px", "10px", "10px", "10px"],
                fill: ["rgba(0,71,255,0.54)"],
                stroke: [0,"rgb(0, 0, 0)","none"]
            },
            {
                id: 'Text',
                type: 'text',
                rect: ['21px', '7px','auto','auto','auto', 'auto'],
                text: "Transparent Rendering<br>",
                align: "center",
                font: ['Arial, Helvetica, sans-serif', 48, "rgba(5,12,251,1.00)", "400", "none", "normal"]
            }],
            symbolInstances: [

            ]
        },
    states: {
        "Base State": {
            "${_RoundRectCopy}": [
                ["color", "background-color", 'rgba(255,151,0,0.54)'],
                ["style", "top", '72px'],
                ["style", "left", '181px'],
                ["transform", "rotateZ", '0deg']
            ],
            "${_RoundRect}": [
                ["color", "background-color", 'rgba(255,0,0,0.54)'],
                ["style", "top", '72px'],
                ["style", "left", '77px'],
                ["transform", "rotateZ", '0deg']
            ],
            "${_RoundRectCopy2}": [
                ["color", "background-color", 'rgba(47,255,0,0.54)'],
                ["style", "top", '72px'],
                ["style", "left", '287px'],
                ["transform", "rotateZ", '0deg']
            ],
            "${_Stage}": [
                ["color", "background-color", 'rgba(0,0,0,0.00)'],
                ["style", "width", '550px'],
                ["style", "height", '344px'],
                ["style", "overflow", 'hidden']
            ],
            "${_Text}": [
                ["style", "top", '7px'],
                ["style", "left", '21px'],
                ["color", "color", 'rgba(5,12,251,1.00)']
            ],
            "${_RoundRectCopy3}": [
                ["color", "background-color", 'rgba(0,71,255,0.54)'],
                ["style", "top", '72px'],
                ["style", "left", '393px'],
                ["transform", "rotateZ", '0deg']
            ]
        }
    },
    timelines: {
        "Default Timeline": {
            fromState: "Base State",
            toState: "",
            duration: 12000,
            autoPlay: true,
            labels: {
                "loop1": 0,
                "loop2": 1000,
                "loop3": 2000,
                "loop4": 3000
            },
            timeline: [
                { id: "eid84", tween: [ "transform", "${_RoundRectCopy2}", "rotateZ", '9deg', { fromValue: '0deg'}], position: 2000, duration: 3000 },
                { id: "eid85", tween: [ "transform", "${_RoundRectCopy2}", "rotateZ", '-9deg', { fromValue: '9deg'}], position: 5000, duration: 3000 },
                { id: "eid86", tween: [ "transform", "${_RoundRectCopy2}", "rotateZ", '0deg', { fromValue: '-9deg'}], position: 8000, duration: 3000 },
                { id: "eid87", tween: [ "transform", "${_RoundRectCopy3}", "rotateZ", '9deg', { fromValue: '0deg'}], position: 3000, duration: 3000 },
                { id: "eid88", tween: [ "transform", "${_RoundRectCopy3}", "rotateZ", '-9deg', { fromValue: '9deg'}], position: 6000, duration: 3000 },
                { id: "eid89", tween: [ "transform", "${_RoundRectCopy3}", "rotateZ", '0deg', { fromValue: '-9deg'}], position: 9000, duration: 3000 },
                { id: "eid81", tween: [ "transform", "${_RoundRectCopy}", "rotateZ", '9deg', { fromValue: '0deg'}], position: 1000, duration: 3000 },
                { id: "eid82", tween: [ "transform", "${_RoundRectCopy}", "rotateZ", '-9deg', { fromValue: '9deg'}], position: 4000, duration: 3000 },
                { id: "eid83", tween: [ "transform", "${_RoundRectCopy}", "rotateZ", '0deg', { fromValue: '-9deg'}], position: 7000, duration: 3000 },
                { id: "eid73", tween: [ "transform", "${_RoundRect}", "rotateZ", '9deg', { fromValue: '0deg'}], position: 0, duration: 3000 },
                { id: "eid74", tween: [ "transform", "${_RoundRect}", "rotateZ", '-9deg', { fromValue: '9deg'}], position: 3000, duration: 3000 },
                { id: "eid75", tween: [ "transform", "${_RoundRect}", "rotateZ", '0deg', { fromValue: '-9deg'}], position: 6000, duration: 3000 }            ]
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
                    rect: ['0px', '0px', '177px', '42px', 'auto', 'auto'],
                    borderRadius: ['10px', '10px', '10px', '10px'],
                    id: 'Normal',
                    stroke: [0, 'rgb(0, 0, 0)', 'none'],
                    type: 'rect',
                    fill: ['rgba(211,203,115,1.00)']
                },
                {
                    type: 'rect',
                    borderRadius: ['10px', '10px', '10px', '10px'],
                    rect: ['0px', '0px', '224px', '65px', 'auto', 'auto'],
                    id: 'Highlight',
                    stroke: [0, 'rgb(0, 0, 0)', 'none'],
                    display: 'none',
                    fill: ['rgba(178,169,192,1.00)']
                },
                {
                    type: 'rect',
                    borderRadius: ['10px', '10px', '10px', '10px'],
                    rect: ['0px', '0px', '224px', '65px', 'auto', 'auto'],
                    id: 'Pressed',
                    stroke: [0, 'rgb(0, 0, 0)', 'none'],
                    display: 'none',
                    fill: ['rgba(138,117,172,1.00)']
                },
                {
                    font: ['Arial, Helvetica, sans-serif', 24, 'rgba(74,73,68,1.00)', '400', 'none', 'normal'],
                    type: 'text',
                    align: 'center',
                    id: 'Label',
                    text: 'Button',
                    textShadow: ['rgba(47,47,47,1.00)', 0, 0, 8],
                    rect: ['0px', '7px', '177px', '35px', 'auto', 'auto']
                },
                {
                    type: 'rect',
                    id: 'EventRect',
                    stroke: [0, 'rgb(0, 0, 0)', 'none'],
                    rect: ['0px', '0px', '177px', '42px', 'auto', 'auto'],
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
                ["style", "width", '177px'],
                ["style", "top", '7px'],
                ["style", "text-align", 'center'],
                ["style", "overflow", 'visible'],
                ["style", "height", '35px'],
                ["subproperty", "textShadow.color", 'rgba(47,47,47,1.00)'],
                ["style", "text-decoration", 'none'],
                ["style", "font-style", 'normal']
            ],
            "${_Highlight}": [
                ["color", "background-color", 'rgba(224,220,160,1.00)'],
                ["style", "height", '42px'],
                ["style", "display", 'none'],
                ["style", "top", '0px'],
                ["style", "left", '0px'],
                ["style", "width", '177px']
            ],
            "${_Normal}": [
                ["color", "background-color", 'rgba(211,203,115,1.00)'],
                ["style", "height", '42px'],
                ["style", "top", '0px'],
                ["style", "left", '0px'],
                ["style", "width", '177px']
            ],
            "${_Pressed}": [
                ["color", "background-color", 'rgba(204,194,90,1.00)'],
                ["style", "height", '42px'],
                ["style", "display", 'none'],
                ["style", "top", '0px'],
                ["style", "left", '0px'],
                ["style", "width", '177px']
            ],
            "${symbolSelector}": [
                ["style", "height", '42px'],
                ["style", "width", '177px']
            ],
            "${_EventRect}": [
                ["style", "height", '42px'],
                ["style", "opacity", '0'],
                ["style", "width", '177px']
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
                { id: "eid32", tween: [ "color", "${_Pressed}", "background-color", 'rgba(204,194,90,1.00)', { animationColorSpace: 'RGB', valueTemplate: undefined, fromValue: 'rgba(204,194,90,1.00)'}], position: 0, duration: 0 },
                { id: "eid46", tween: [ "style", "${_Pressed}", "height", '42px', { fromValue: '42px'}], position: 0, duration: 0 },
                { id: "eid48", tween: [ "style", "${_Highlight}", "height", '42px', { fromValue: '42px'}], position: 0, duration: 0 },
                { id: "eid47", tween: [ "style", "${_Pressed}", "width", '177px', { fromValue: '177px'}], position: 0, duration: 0 },
                { id: "eid49", tween: [ "style", "${_Highlight}", "width", '177px', { fromValue: '177px'}], position: 0, duration: 0 },
                { id: "eid16", tween: [ "style", "${_Highlight}", "display", 'none', { fromValue: 'none'}], position: 0, duration: 0 },
                { id: "eid14", tween: [ "style", "${_Highlight}", "display", 'block', { fromValue: 'none'}], position: 1000, duration: 0 },
                { id: "eid19", tween: [ "style", "${_Highlight}", "display", 'none', { fromValue: 'block'}], position: 2000, duration: 0 },
                { id: "eid31", tween: [ "color", "${_Highlight}", "background-color", 'rgba(224,220,160,1.00)', { animationColorSpace: 'RGB', valueTemplate: undefined, fromValue: 'rgba(224,220,160,1.00)'}], position: 0, duration: 0 },
                { id: "eid17", tween: [ "style", "${_Pressed}", "display", 'none', { fromValue: 'none'}], position: 0, duration: 0 },
                { id: "eid15", tween: [ "style", "${_Pressed}", "display", 'none', { fromValue: 'none'}], position: 1000, duration: 0 },
                { id: "eid18", tween: [ "style", "${_Pressed}", "display", 'block', { fromValue: 'none'}], position: 2000, duration: 0 }            ]
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
})(jQuery, AdobeEdge, "EDGE-1907987303");
