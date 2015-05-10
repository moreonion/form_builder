<?php

class FormBuilderWebformType extends FormBuilderFormBase {

  /**
   * {@inheritdoc}
   */
  public function storeProperty(&$element, $property, $value) {
    $component = &$element['#webform_component'];
    $this->properties[$property]->setValue($component, $value);
  }

  /**
   * {@inheritdoc}
   */
  public function renderElement($element) {
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
  public function __construct($property, $params) {
    $params += array('storage_parents' => array($property));
    parent::__construct($property, $params);
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
