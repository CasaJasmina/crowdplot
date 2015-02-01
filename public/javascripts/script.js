var activeCellX;
var activeCellY;
var activeArray;

$(document).ready(function(){
	




	$("#hover").hide();
	$(".loading").hide();

	$(".tableCell").click(function() {
		activeCellX=$(this).attr('x');
		activeCellY=$(this).attr('Y');
		$("#hover").show();
		$('body').addClass('stop-scrolling')

		$("#activeCell").text("x="+activeCellX+" ,y="+activeCellY );
		console.log(activeCellY);

	});


	var s = Snap("#svg");
	//	var bigCircle = s.circle(150, 150, 100);
	//	var p1 = s.polyline(10, 10, 10, 100, 210, 20, 101, 120);

	var mousepressed=false;

	var lineStarted=false;
	var lastPointX;
	var lastPointX;

	var curveIndex=0;

	var curves=[];

	var gcode="";

	var pDown=70;
	var pUp=90;

	$("#svg").on( "mousemove touchmove", function( event ) {
		$( "#log1" ).text( "pageX: " + event.pageX + ", pageY: " + event.pageY );

		if (mousepressed){
			var x = event.pageX-$("#svg").offset().left;
			var y = event.pageY-$("#svg").offset().top;

			$( "#log2" ).text( "svgx: " + x + ", svgy: " + y );

			if(!lineStarted){
				lineStarted=true;
				curves[curveIndex]=s.g();
				curves[curveIndex].attr({
					id:curveIndex,	
					stroke: "#000",
					strokeWidth: 2,
					'stroke-linecap':"round"
				});

				console.log("lineStarted");
				lastPointX=x;
				lastPointY=y;

				var moveToPos='G90 \n';
				gcode=gcode.concat(moveToPos);

				var moveToPos='F100 \n';
				gcode=gcode.concat(moveToPos);

				var moveToPos='G00 X'+x/4+' Y'+-y/4+' \n';
				//console.log(moveToPos);
				gcode=gcode.concat(moveToPos);
				var penDown='G00 Z'+pDown+' \n'; //
				gcode=gcode.concat(penDown);



			}else{
					//console.log("drawing line"+","+lastPointX+","+lastPointY+","+x+","+y);
					curves[curveIndex].add(s.line(lastPointX,lastPointY,x,y));
					lastPointX=x;
					lastPointY=y;


					var moveToPos='G00 X'+x/4+' Y'+-y/4+' \n';
					gcode=gcode.concat(moveToPos);

				}
			}
		});


$("#svg").on( "mousedown touchstart", function( event ) {
	mousepressed=true;
	console.log("mousepressed");
});


$("#svg").on( "mouseup touchend", function( event ) {
	mousepressed=false;	
	lineStarted=false;
	curveIndex++;
	console.log("lineEnded");
	var penUp='G00 Z'+pUp+' \n'; //
	gcode=gcode.concat(penUp);
	console.log(gcode);
});



$(".undo").click(function(){
	console.log ("back");
	curves[curveIndex-1].remove();
	curveIndex--;
});

$(".back").click(function(){
	$("#hover").hide();
	$('body').removeClass('stop-scrolling')
});



$(".save").click(function(){
	var svg= $("#svgWrap").html()
		//console.log(svg);

		$.post( "save", { 
			content: svg, 
			positionX:activeCellX, 
			positionY:activeCellY, 
			gcode:gcode
		},function(){
			$(".loading").show();
		}) 
		
		

		.done(function( data ) {
			$("#hover").hide();

			//alert( "Data Loaded: " + data );
			var d = new Date();
			$("#hover").hide();
			$('body').removeClass('stop-scrolling')


			$("#"+activeCellX+"_"+activeCellY+" img").attr({
				src: 'uploads/png/'+activeCellX+'_'+activeCellY+'_mini.png?'+d.getTime(),
			});
		});




	});



});