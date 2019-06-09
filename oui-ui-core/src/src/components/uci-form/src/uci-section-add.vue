<template>
  <div v-if="visible">
    <el-button type="primary" size="mini" @click="add">Add</el-button>
  </div>
</template>

<script>
export default {
  name: 'UciSectionAdd',
  props: {
    sestion: Object
  },
  computed: {
    visible() {
      const s = this.sestion;
      return s.addremove && s.type && !s.name;
    }
  },
  methods: {
    add() {
      this.sestion.add();
    }
  }
}
</script>
