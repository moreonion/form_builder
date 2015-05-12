<?php

class FormBuilderWebformElement extends FormBuilderElementBase {

  /**
   * {@inheritdoc}
   */
  protected function setProperty($property, $value) {
    $component = &$this->element['#webform_component'];
    $properties = $this->getProperties();
    $properties[$property]->setValue($component, $value);
  }

  /**
   * {@inheritdoc}
   */
  public function render() {
    $element = &$this->element;
    if (isset($element['#webform_component'])) {
      $component = &$element['#webform_component'];
      $new_element = webform_component_invoke($component['type'], 'render', $component, NULL, FALSE);
      return $new_element + $element;
    }
    return $element;
  }

}

class FormBuilderWebformProperty extends FormBuilderPropertyBase {

  protected $storageParents;

  /**
   * {@inheritdoc}
   */
  public function __construct($property, $params, $form_type_name) {
    $params += array('storage_parents' => array($property));
    parent::__construct($property, $params, $form_type_name);
    $this->storageParents = $params['storage_parents'];
  }

  /**
   * {@inheritdoc}
   */
  public function setValue(&$component, $value) {
    drupal_array_set_nested_value($component, $this->storageParents, $value);
  }
}

/**
 * Special handling for the mandatory -> required rename in webform4.
 */
class FormBuilderWebformPropertyRequired extends FormBuilderPropertyBase {
  public function setValue(&$component, $value) {
    $component['required'] = $value; // webform 4
    $component['mandatory'] = $value; // webform 3
  }
}

/**
 * Special handling for $component['value'].
 */
class FormBuilderWebformPropertyValue extends FormBuilderWebformProperty {
  /**
   * {@inheritdoc}
   */
  public function setValue(&$component, $value) {
    if (is_array($value)) {
      $value = implode(',', $value);
    }
    parent::setValue($component, $value);
  }
}

