var activeCellX;
var activeCellY;
var activeArray;

$(document).ready(function(){
	

	$(".tableCell").click(function() {
		activeCellX=$(this).attr('x');
		activeCellY=$(this).attr('Y');

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

	var pDown=80;
	var pUp=100;

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

				var moveToPos='G00 X'+x/10+' Y'+-y/10+' \n';
				//console.log(moveToPos);
				gcode=gcode.concat(moveToPos);
				var penDown='G00 Z'+pDown+' \n'; //
				gcode=gcode.concat(penDown);



			}else{
					//console.log("drawing line"+","+lastPointX+","+lastPointY+","+x+","+y);
					curves[curveIndex].add(s.line(lastPointX,lastPointY,x,y));
					lastPointX=x;
					lastPointY=y;


					var moveToPos='G00 X'+x/10+' Y'+-y/10+' \n';
					gcode=gcode.concat(moveToPos);

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

		
		var penUp='G00 Z'+pUp+' \n'; //
		gcode=gcode.concat(penUp);
		console.log(gcode);
	});


	$(".back").click(function(){
		console.log ("back");
		curves[curveIndex-1].remove();
		curveIndex--;
	});


	$(".save").click(function(){
		var svg= $("#svgWrap").html()
		//console.log(svg);

		$.post( "save", { content: svg, positionX:activeCellX, positionY:activeCellY, gcode:gcode })

		.done(function( data ) {
			//alert( "Data Loaded: " + data );
			var d = new Date();

			$("#"+activeCellX+"_"+activeCellY+" img").attr({
				src: 'uploads/png/'+activeCellX+'_'+activeCellY+'_mini.png?'+d.getTime(),
			});
		});


	});



});