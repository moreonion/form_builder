<?php


class FormBuilderWebformForm extends FormBuilderFormBase {
  /**
   * Create a webform component array based the form_builder cache.
   *
   * @param string $element_id
   *   Unique ID of the element.
   * @return array
   *   A webform component array.
   */
  public function getComponent($element_id) {
    module_load_include('inc', 'form_builder_webform', 'form_builder_webform.components');

    $element = $this->getElementArray($element_id);
    $component = $element['#webform_component'];
    $type = $component['type'];

    $component['email'] = 0;
    $component['nid'] = $this->formId;
    $component['weight'] = $element['#weight'];
    // The component can't decide this on it's own.
    $component['pid'] = 0;

    // Allow each component to set any specific settings that can't be mapped.
    if ($saved_component = form_builder_webform_component_invoke($type, 'form_builder_save', $component, $element)) {
      $component = $saved_component;
    }

    return $component;
  }

  /**
   * Get a list of component-arrays just like in $node->webform['components'].
   */
  public function getComponents($node) {
    // Max CID is used in the creation of new components, preventing conflicts.
    $cids = array();
    // Filter out all cids from removed components.
    foreach (array_keys($node->webform['components']) as $cid) {
      if ($this->getElement("cid_$cid")) {
        $cids[] = $cid;
      }
    }
    $max_cid = max($cids);

    $components = array();
    // Keep track of the element_id => cid mapping for assigning the pid.
    $element_cid_map = array();
    $page = 1;
    $elements = $this->getElementArraysInPreOrder();
    foreach ($elements as $element_id => $element) {
      if ($component = $this->getComponent($element_id)) {
        if (empty($component['cid'])) {
          $component['cid'] = ++$max_cid;
        }
        $element_cid_map[$element_id] = $component['cid'];

        // Set the possibly changed pid.
        $parent_id = $element['#form_builder']['parent_id'];
        $component['pid'] = ($parent_id === FORM_BUILDER_ROOT) ? 0 : $element_cid_map[$parent_id];
        $component['page_num'] = $page;

        $components[$component['cid']] = $component;
        if ($component['type'] == 'pagebreak') {
          $page++;
        }
      }
    }
    return $components;
  }
}

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
      // Preserve the #weight. It may have been changed by the positions form.
      $new_element['#weight'] = $element['#weight'];
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

  /**
   * {@inheritdoc}
   */
  public function form(&$form_state, $element) {
    $p["#{$this->property}"] = $this->getValue($element['#webform_component'])
      + $element;
    return parent::form($form_state, $p);
  }

  /**
   * Read the value from a component array.
   */
  public function getValue($component) {
    return drupal_array_get_nested_value($component, $this->storageParents);
  }

}

/**
 * Special handling for the mandatory -> required rename in webform4.
 */
class FormBuilderWebformPropertyRequired extends FormBuilderWebformProperty {
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

