<template>
  <UciValue :title="title" :name="name" :value="value" v-slot="{_value}" ref="UciValue">
    <Input v-for="v in _value" :key="v" :value="v" readonly icon="md-close" @on-click="onDelClick(v)" class="UciDynamicListDel" />
    <Input icon="md-add" v-model="addValue"  @on-click="onAddClick" class="UciDynamicListAdd" />
  </UciValue>
</template>

<script>

export default {
  name: 'UciDynamicList',
  props: {
    name: String,
    title: String,
    value: String
  },
  data() {
    return {
      addValue: ''
    }
  },
  methods: {
    onDelClick(v) {
      const values = [];
      this.$refs.UciValue.$children[0].$children.forEach(item => {
        if (item.value !== '' && item.value !== v)
          values.push(item.value);
      });
      this.$refs.UciValue._value = values;
    },
    onAddClick() {
      const values = [];
      this.$refs.UciValue.$children[0].$children.forEach(item => {
        if (item.value !== '')
          values.push(item.value);
      });
      this.addValue = '';
      this.$refs.UciValue._value = values;
    }
  }
}
</script>

<style>
  .UciDynamicListDel .ivu-input-icon {
    color: red;
  }

  .UciDynamicListAdd .ivu-input-icon {
    color: #2d8cf0;
  }
</style>
