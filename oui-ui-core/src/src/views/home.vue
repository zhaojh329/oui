<template>
  <div>
    <CardList title="Device information" :list="devinfo"></CardList>
  </div>
</template>

<script>

export default {
  data() {
    return {
      devinfo: []
    }
  },
  created() {
    this.$system.getBoardInfo().then(r => {
      this.devinfo = [
        ['Model', r.model],
        ['Architecture', r.system],
        ['Kernel Version', r.kernel],
        ['Firmware Version', r.release.revision]
      ];
    });
  }
}
</script>
