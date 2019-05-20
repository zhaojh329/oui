<template>
  <div id="app">
    <router-view />
  </div>
</template>

<script>
export default {
  name: 'App',
  mounted() {
    this.$session.isAlive().then(alive => {
      if (alive) {
        this.$session.startHeartbeat();
      }
    });
  }
}
</script>

<style lang="less">
.size {
  width: 100%;
  height: 100%;
}

html, body {
  .size;
  overflow: hidden;
  margin: 0;
  padding: 0;
}

#app {
  .size;
}
</style>
