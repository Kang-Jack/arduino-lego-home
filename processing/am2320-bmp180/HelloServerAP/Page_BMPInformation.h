
//
//   The HTML PAGE
//
const char BMP_Information[] PROGMEM = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<link rel="stylesheet" href="style.css" type="text/css" />
<script src="microajax.js"></script> 
<a href="admin.html"  class="btn btn--s"><</a>&nbsp;&nbsp;<strong>Chip = bmp180 </strong>
<hr>
<table border="0"  cellspacing="0" cellpadding="3" style="width:310px" >
<tr><td align="right">Temperature :</td><td><span id="x_temp"></span> Celsius</td></tr>
<tr><td align="right">Pressure :</td><td><span id="x_press"></span> Pascal</td></tr>
<tr><td align="right">Altitude :</td><td><span id="x_alti"></span> meters</td></tr>
<tr><td align="right">Pressure at sealevel :</td><td><span id="x_sl_press"></span> Pa</td></tr>
<tr><td align="right">Real altitude:</td><td><span id="x_real_alti"></span> meters</td></tr>
<tr><td colspan="2" align="center"><a href="javascript:GetState()" class="btn btn--m btn--blue">Refresh</a></td></tr>
</table>
<script>

function GetState()
{
	setValues("/bmpvalues");
}

window.onload = function ()
{
	load("style.css","css", function() 
	{
		load("microajax.js","js", function() 
		{
				GetState();
		});
	});
}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}



</script>
)=====" ;





