<template>
  <div>
    <CardList title="Device information" :list="devinfo"></CardList>
  </div>
</template>

<script>

export default {
  name: 'home',
  data() {
    return {
      devinfo: []
    }
  },
  mounted() {
    this.$ubus.call('system', 'board').then(r => {
      this.devinfo = [
        ['Model', r.model],
        ['Architecture', r.system],
        ['Kernel Version', r.kernel],
        ['Firmware Version', r.release.revision]
      ];

      document.title = r.hostname + ' - oui';
    });
  }
}
</script>
