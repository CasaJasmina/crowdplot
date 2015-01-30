var activeCellX;
var activeCellY;
var activeArray;

$(document).ready(function(){
	

	$(".tableCell").click(function() {
		activeCellX=$(this).attr('x');
		activeCellY=$(this).attr('Y');

		$("#activeCell").text("x="+activeCellX+" ,y="+activeCellY );

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

	$("#svg").on( "mousemove", function( event ) {
		$( "#log1" ).text( "pageX: " + event.pageX + ", pageY: " + event.pageY );

		if (mousepressed){
			var x = event.pageX-$("#hover").offset().left;
			var y = event.pageY-$("#hover").offset().top;

			$( "#log2" ).text( "svgx: " + x + ", svgy: " + y );

			if(!lineStarted){
				lineStarted=true;
				curves[curveIndex]=s.g();
				curves[curveIndex].attr({
					id:curveIndex,	
					stroke: "#000",
					strokeWidth: 10,
					'stroke-linecap':"round"
				});
				console.log("lineStarted");
				lastPointX=x;
				lastPointY=y;
			}else{
					//console.log("drawing line"+","+lastPointX+","+lastPointY+","+x+","+y);
					curves[curveIndex].add(s.line(lastPointX,lastPointY,x,y));
					lastPointX=x;
					lastPointY=y;
				}
			}
		});


	$("#svg").mousedown(function(){
		mousepressed=true;
	});

	$("#svg").mouseup(function(){
		mousepressed=false;	
		lineStarted=false;
		curveIndex++;
		console.log("lineEnded");

	});

	$(".back").click(function(){
		console.log ("back");
		curves[curveIndex-1].remove();
		curveIndex--;
	});


	$(".save").click(function(){
		var svg= $("#svgWrap").html()
		//console.log(svg);

		$.post( "save", { content: svg, positionX:activeCellX, positionY:activeCellY })

		.done(function( data ) {
			//alert( "Data Loaded: " + data );
			var d = new Date();

			$("#"+activeCellX+"_"+activeCellY+" img").attr({
				src: 'uploads/newSvg'+activeCellX+'_'+activeCellY+'_mini.png?'+d.getTime(),
			});
		});


	});



});