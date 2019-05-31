<template>
  <div id="app">
    <router-view></router-view>
  </div>
</template>

<style lang="scss">
html, body, #app {
  height: 100%;
}

body {
	margin: 0px;
	padding: 0px;
	font-family: Microsoft YaHei, Helvetica Neue, Helvetica, PingFang SC, Hiragino Sans GB,  SimSun, sans-serif;
	font-size: 14px;
	-webkit-font-smoothing: antialiased;
	background-color: #f2f2f2;
  color: #333333;
}

#app {
	position: absolute;
	top: 0px;
	bottom: 0px;
	width: 100%;
}

a {
	color: #56a9ff;
	text-decoration: none;
}

</style>
