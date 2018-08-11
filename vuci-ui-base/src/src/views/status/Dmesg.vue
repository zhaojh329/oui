<template>
  <v-textarea solo auto-grow readonly :value="log"></v-textarea>
</template>

<script>
  export default {
    data: () => ({
      log: ''
    }),
    mounted() {
      this.$ubus.call('vuci.system', 'dmesg').then(r => {
        this.log = r.log;
      });
    }
  };
</script>