<?php
	require_once "fitsdb.php";

	hlavicka ("login", "login", "", "ok");
?>
	<form action="index.php">
		<table>
			<tr>
				<td valign="left">Login:</td>
				<td><input type="text" name="login"></input></td>
			</tr><tr>
				<td valign="left">Password:</td>
				<td><input type="password" name="password"></input></td>
			</tr>
		</table>

		<input type="submit" value="Submit"></input>
	</form>
<?php
	konec ();
?>
