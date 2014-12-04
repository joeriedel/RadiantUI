/***********************
* Adobe Edge Animate Composition Actions
*
* Edit this file with caution, being careful to preserve 
* function signatures and comments starting with 'Edge' to maintain the 
* ability to interact with these actions from within Adobe Edge Animate
*
***********************/
(function($, Edge, compId){
var Composition = Edge.Composition, Symbol = Edge.Symbol; // aliases for commonly used Edge classes

   //Edge symbol: 'stage'
   (function(symbolName) {
      
      
      Symbol.bindSymbolAction(compId, symbolName, "creationComplete", function(sym, e) {
         $("<link rel='stylesheet' type='text/css' href='style.css'>").appendTo("#Stage");
         $("<script type='text/javascript' src='radiantui-null.js'></script>").appendTo("#Stage");
         var Title = sym.$("Text3");
         Title.css("-webkit-user-select", "none");
         Title.css("-webkit-tap-highlight-color", "rgba(0,0,0,0)");

      });
      //Edge binding end

      Symbol.bindElementAction(compId, symbolName, "${_NewGame}", "mouseup", function(sym, e) {
         RadiantUI.TriggerEvent("Received_NewGameButtonClicked");

      });
      //Edge binding end

      Symbol.bindElementAction(compId, symbolName, "${_NewGameCopy2}", "mouseup", function(sym, e) {
         RadiantUI.TriggerEvent("Received_ExitGameButtonClicked");

      });
      //Edge binding end

   })("stage");
   //Edge symbol end:'stage'

   //=========================================================
   
   //Edge symbol: 'MainMenuButton_S'
   (function(symbolName) {   
   
      Symbol.bindSymbolAction(compId, symbolName, "creationComplete", function(sym, e) {
         var Label = sym.$("Label");
         Label.html(sym.getSymbolElement().attr("title"));
         Label.css("-webkit-user-select", "none");
         Label.css("-webkit-tap-highlight-color", "rgba(0,0,0,0)");
         

      });
      //Edge binding end

      Symbol.bindElementAction(compId, symbolName, "${_EventRect}", "mouseout", function(sym, e) {
         sym.stop("Normal");

      });
      //Edge binding end

      Symbol.bindElementAction(compId, symbolName, "${_EventRect}", "mouseover", function(sym, e) {
         sym.stop("Highlight");
         RadiantUI.TriggerEvent("JSEvent_MainMenuButtonHighlight");

      });
      //Edge binding end

      Symbol.bindElementAction(compId, symbolName, "${_EventRect}", "mousedown", function(sym, e) {
         sym.stop("Pressed")

      });
      //Edge binding end

      Symbol.bindElementAction(compId, symbolName, "${_EventRect}", "mouseup", function(sym, e) {
         sym.stop("Highlight");

      });
      //Edge binding end

   })("MainMenuButton_S");
   //Edge symbol end:'MainMenuButton_S'

})(jQuery, AdobeEdge, "EDGE-1907987303");